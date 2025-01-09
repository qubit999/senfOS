#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/platform.h"
#include "pico/rand.h"
#include <pico/flash.h>
#include "hardware/structs/rosc.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "lwip/altcp_tls.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/fs.h"

#include "mbedtls/entropy.h"

#include "certs/certs.h"
#include "myhttpserver.h"
#include "../settings.h"
#include "oled.h"
// #include "epd_213.h"

#define GB_NAME_LEN 64
#define GB_MESSAGE_LEN 256

struct GuestbookEntry {
    std::string name;
    std::string message;
};

std::vector<GuestbookEntry> guestbook_entries;

static char stored_key1[64];
static char stored_value1[64];
static char stored_key2[64];
static char stored_value2[64];

struct altcp_pcb *pcb = NULL;

void initialize_pcb() {
    pcb = altcp_tcp_new();
    if(!pcb) {
        printf("Failed to create new TCP PCB\n");
        return;
    }
}

Pico_OLED_1_3 MyHTTPServer::oled;
// epd_213 MyHTTPServer::epd;

struct altcp_tls_config *tls_config = NULL;

typedef int64_t mbedtls_ms_time_t;

mbedtls_ms_time_t mbedtls_ms_time(void) {
    return (mbedtls_ms_time_t)(time_us_64() / 1000);
}

/* Function to feed mbedtls entropy. */
int mbedtls_hardware_poll(void *data __unused, 
        unsigned char *output, size_t len, size_t *olen) {
 *olen = 0;
 while(*olen < len) {
    uint64_t rand_data = get_rand_64();
    size_t to_copy = MIN(len, sizeof(rand_data));
    memcpy(output + *olen, &rand_data, to_copy);
    *olen += to_copy;
 }
 return 0;
}

void sntp_set_system_time(uint32_t sec, uint32_t us)
{
  struct tm info;
  time_t tim = sec;
  struct timeval tmv;
  tmv.tv_sec=sec;
  tmv.tv_usec=us;
  settimeofday(&tmv, 0);
  printf("set time :%s\n", ctime(&tim));
  sntp_stop();
}

std::string escape_html(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '&': escaped += "&amp;"; break;
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&apos;"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string url_decode(const std::string &src) {
    std::string decoded;
    char a, b;
    for (size_t i = 0; i < src.length(); i++) {
        if ((src[i] == '%') && ((a = src[i + 1]) && (b = src[i + 2])) && (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            decoded += 16 * a + b;
            i += 2;
        } else if (src[i] == '+') {
            decoded += ' ';
        } else {
            decoded += src[i];
        }
    }
    return decoded;
}

static u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen, u16_t current_tag_part, u16_t *next_tag_part) {
    printf("SSI handler called with index: %d\n", iIndex);

    const char *value_to_insert = NULL;
    size_t value_len = 0;

    static std::string entries_html;

    switch (iIndex) {
        case 0: value_to_insert = stored_key1; break;
        case 1: value_to_insert = stored_value1; break;
        case 2: value_to_insert = stored_key2; break;
        case 3: value_to_insert = stored_value2; break;
        case 4: {
            // Generate HTML for guestbook entries
            entries_html.clear();
            // check if guestbook_entries[0].name is empty and guestbook_entries[0].message is empty
            if (guestbook_entries.size() == 0) {
                entries_html = "<p>No guestbook entries.</p>";
                value_to_insert = entries_html.c_str();
            } else {
                for (const GuestbookEntry& entry : guestbook_entries) {
                    entries_html += "<p><b>" + escape_html(entry.name) + "</b>: " + escape_html(entry.message) + "</p>";
                }
            }
            value_to_insert = entries_html.c_str();
            break;
        }
        default:
            printf("SSI: Unknown index\n");
            return 0;
    }

    value_len = strlen(value_to_insert);
    if (value_len > iInsertLen) {
        value_len = iInsertLen;
    }

    strncpy(pcInsert, value_to_insert, value_len);
    pcInsert[value_len] = '\0';

    printf("SSI: Inserting %s = '%s'\n", 
           (iIndex % 2 == 0) ? "Key" : "Value", 
           pcInsert);

    return value_len;
}


err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                    u16_t http_request_len, int content_len,
                    char *response_uri, u16_t response_uri_len, u8_t *post_auto_wnd)
{
    printf("POST request started for URI: %s\n", uri);
    return ERR_OK;
};

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    printf("Received POST data of length %d\n", p->tot_len);

    char post_data[128] = {0};
    pbuf_copy_partial(p, post_data, sizeof(post_data) - 1, 0);
    printf("Raw POST data: %s\n", post_data);

    if (p->tot_len >= sizeof(post_data)) {
        printf("POST data too large for buffer\n");
        pbuf_free(p);
        return ERR_MEM;
    }

    if (p->tot_len == 0) {
        printf("No POST data received\n");
        pbuf_free(p);
        return ERR_OK;
    }

    // Parse the data as URL-encoded form
    char *name_start = strstr(post_data, "name=");
    char *message_start = strstr(post_data, "message=");
    if (name_start && message_start) {
        name_start += 5;  // Skip "name="
        message_start += 8;  // Skip "message="

        char *name_end = strchr(name_start, '&');
        char *message_end = strchr(message_start, '&');

        std::string name(name_start, name_end ? name_end - name_start : strlen(name_start));
        std::string message(message_start, message_end ? message_end - message_start : strlen(message_start));

        // URL decode the name and message
        name = url_decode(name);
        message = url_decode(message);

        guestbook_entries.push_back({name, message});
        printf("Guestbook entry added: Name=%s, Message=%s\n", name.c_str(), message.c_str());
    }

    if (strstr(post_data, "msg=") != NULL) {
        // Parse the data as a URL-encoded form
        char *msg_start = strstr(post_data, "msg=");
        if (msg_start) {
            msg_start += 4;  // Skip "msg="
            char *end = strchr(msg_start, '&');
            if (end) *end = '\0';
            printf("Parsed message: %s\n", msg_start);
            if (end) *end = '&';
        }
        if (!msg_start) {
            printf("No message found in POST data\n");
            pbuf_free(p);
            return ERR_OK;
        }
        if (strlen(msg_start) > 32) {
            printf("Message too long\n");
            pbuf_free(p);
            return ERR_ARG;
        }
        std::string msg = msg_start;
        std::replace(msg.begin(), msg.end(), '+', ' ');
        const char* msg_cstr = msg.c_str();
        if(DISPLAY_MODULE == 1) {
            MyHTTPServer::oled.drawPage("MESSAGE RX", "MESSAGE", msg_cstr, "", 12);
        } else if(DISPLAY_MODULE == 2) {
            // MyHTTPServer::epd.drawPage("MESSAGE RX", "MESSAGE", msg_cstr, "", 12);
        }
        pbuf_free(p);
        return ERR_OK;
    }

    char *value1_start = strstr(post_data, "value1=");
    char *value2_start = strstr(post_data, "value2=");
    
    if (value1_start) {
        value1_start += 7;  // Skip "value1="
        char *end = strchr(value1_start, '&');
        if (end) *end = '\0';
        strncpy(stored_key1, "Value1", sizeof(stored_key1) - 1);
        strncpy(stored_value1, value1_start, sizeof(stored_value1) - 1);
        if (end) *end = '&';
    }
    
    if (value2_start) {
        value2_start += 7;  // Skip "value2="
        char *end = strchr(value2_start, '&');
        if (end) *end = '\0';
        strncpy(stored_key2, "Value2", sizeof(stored_key2) - 1);
        strncpy(stored_value2, value2_start, sizeof(stored_value2) - 1);
    }

    printf("Parsed values - Key1: %s, Value1: %s, Key2: %s, Value2: %s\n", 
           stored_key1, stored_value1, stored_key2, stored_value2);
    
    pbuf_free(p);
    return ERR_OK;
};

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
        // Finalize POST request processing
        printf("POST request finished\n");
        strncpy(response_uri, "/post.html", response_uri_len);
};

static err_t http_recv_function(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (!p) {
        printf("Connection closed by client\n");
        altcp_close(pcb);
        return ERR_OK;
    }

    // Process received data
    char buffer[128];
    size_t copy_len = MIN(p->tot_len, sizeof(buffer) - 1);
    pbuf_copy_partial(p, buffer, copy_len, 0);
    buffer[copy_len] = '\0';
    
    printf("Received data: %s\n", buffer);
    
    // Echo data back to client (optional)
    err_t write_err = altcp_write(pcb, buffer, strlen(buffer), TCP_WRITE_FLAG_COPY);
    if (write_err != ERR_OK) {
        printf("Error writing response: %d\n", write_err);
    }
    
    pbuf_free(p); // Free received data buffer
    return ERR_OK;
}


static void http_error_function(void *arg, err_t err) {
    printf("Connection error: %d\n", err);
};

static err_t http_poll_function(void *arg, struct altcp_pcb *pcb) {
    printf("Polling connection\n");
    return ERR_OK;
};

static err_t http_accept_function(void *arg, struct altcp_pcb *newpcb, err_t err) {
    if (err != ERR_OK || newpcb == NULL) {
        return ERR_VAL;
    }
    printf("New connection accepted\n");
    // Set up callbacks for this connection
    altcp_recv(newpcb, http_recv_function);
    altcp_err(newpcb, http_error_function);
    altcp_poll(newpcb, http_poll_function, 4);
    return ERR_OK;
}

MyHTTPServer::MyHTTPServer() {
    initialize_pcb();
    this->start();
}
MyHTTPServer::~MyHTTPServer() {}

void MyHTTPServer::start() {
    printf("Starting HTTP server\n");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();

    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);
    if (mbedtls_entropy_add_source(&entropy, mbedtls_hardware_poll, NULL, 64, MBEDTLS_ENTROPY_SOURCE_STRONG) != 0) {
        printf("Failed to add hardware entropy source\n");
        return;
    }

    const u8_t *pass = (const u8_t *)PRIVKEY_PASS;
    size_t pass_len = strlen((const char*)pass);

    this->tls_config = altcp_tls_create_config_server_privkey_cert(
        (const u8_t *)server_key, server_key_len, 
        pass, pass_len,
        (const u8_t *)server_cert, server_cert_len
    );

    if (!this->tls_config) {
        printf("Failed to create TLS config\n");
        return;
    }

    // Update SSI tags to match exactly what's in the HTML
    static const char *ssi_tags[] = {
        "Key1",     // <!--#Key1-->
        "Value1",   // <!--#Value1--> 
        "Key2",     // <!--#Key2-->
        "Value2",    // <!--#Value2-->
        "Entries1", // <!--#Entries1-->
    };

    http_set_ssi_handler(ssi_handler, ssi_tags, 
                        sizeof(ssi_tags) / sizeof(ssi_tags[0]));
    printf("SSI handler and tags registered\n");

    this->start_cgi();

    httpd_inits(tls_config);
    // err_t err = altcp_bind(pcb, IP_ANY_TYPE, HTTPD_SERVER_PORT_HTTPS);
    // if (err != ERR_OK) {
    //     printf("Failed to bind TCP PCB: %d\n", err);
    //     return;
    // }

    // pcb = altcp_listen(pcb);
    // if (!pcb) {
    //     printf("Failed to listen on TCP PCB\n");
    //     return;
    // }

    // altcp_accept(pcb, http_accept_function);
    // altcp_poll(pcb, http_poll_function, 4);
    // printf("HTTP server is listening on port %d\n", HTTPD_SERVER_PORT_HTTPS);
};


void MyHTTPServer::start_cgi() {
    static const tCGI cgi_handlers[] = {
        {"/", [](int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) -> const char* {
            MyHTTPServer::debugInfo(iIndex, iNumParams, pcParam, pcValue);
            return "/index.shtml";
        }},
        {"/post", [](int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) -> const char* {
            MyHTTPServer::debugInfo(iIndex, iNumParams, pcParam, pcValue);
            return "/post.html";
        }},
        {"/get", [](int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) -> const char* {
            // Store keys and values from GET parameters
            for (int i = 0; i < iNumParams; i++) {
                if (i == 0) {
                    strncpy(stored_key1, pcParam[i], sizeof(stored_key1) - 1);
                    strncpy(stored_value1, pcValue[i], sizeof(stored_value1) - 1);
                } else if (i == 1) {
                    strncpy(stored_key2, pcParam[i], sizeof(stored_key2) - 1);
                    strncpy(stored_value2, pcValue[i], sizeof(stored_value2) - 1);
                }
            }
            MyHTTPServer::debugInfo(iIndex, iNumParams, pcParam, pcValue);
            return "/get.shtml";
        }},
        {"/sendMessage", [](int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) -> const char* {
            MyHTTPServer::debugInfo(iIndex, iNumParams, pcParam, pcValue);
            return "/post.html";
        }},
    };
    http_set_cgi_handlers(cgi_handlers, sizeof(cgi_handlers) / sizeof(cgi_handlers[0]));
};

void MyHTTPServer::debugInfo(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    printf("Debug info\n");
    printf("Index: %d\n", iIndex);
    printf("NumParams: %d\n", iNumParams);
    for (int i = 0; i < iNumParams; i++) {
        printf("Param[%d]: %s\n", i, pcParam[i]);
        printf("Value[%d]: %s\n", i, pcValue[i]);
    }
    printf("\n");
};