#include "myhttpserver.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>
#include "lwip/apps/httpd.h"
#include "lwip/apps/fs.h"
#include "oled.h"

static char stored_key1[64];
static char stored_value1[64];
static char stored_key2[64];
static char stored_value2[64];

Pico_OLED_1_3 MyHTTPServer::oled;

static u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen, u16_t current_tag_part, u16_t *next_tag_part) {
    printf("SSI handler called with index: %d\n", iIndex);

    stored_key1[sizeof(stored_key1) - 1] = '\0';
    stored_value1[sizeof(stored_value1) - 1] = '\0';
    stored_key2[sizeof(stored_key2) - 1] = '\0';
    stored_value2[sizeof(stored_value2) - 1] = '\0';

    switch (iIndex) {
        case 0: // Key1
            strcpy(pcInsert, stored_key1);
            printf("SSI: Inserting Key1 = '%s'\n", stored_key1);
            return strlen(stored_key1);
        case 1: // Value1  
            strcpy(pcInsert, stored_value1);
            printf("SSI: Inserting Value1 = '%s'\n", stored_value1);
            return strlen(stored_value1);
        case 2: // Key2
            strcpy(pcInsert, stored_key2);
            printf("SSI: Inserting Key2 = '%s'\n", stored_key2);
            return strlen(stored_key2);
        case 3: // Value2
            strcpy(pcInsert, stored_value2);
            printf("SSI: Inserting Value2 = '%s'\n", stored_value2);
            return strlen(stored_value2);
        default:
            printf("SSI: Unknown index\n");
            return 0;
    }
}

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                    u16_t http_request_len, int content_len,
                    char *response_uri, u16_t response_uri_len, u8_t *post_auto_wnd)
{
    printf("POST request started for URI: %s\n", uri);
    return ERR_OK;
}

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
        MyHTTPServer::oled.drawPage("MESSAGE RX", "MESSAGE", msg_cstr, "", 12);
        sleep_ms(3000);
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
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
        // Finalize POST request processing
        printf("POST request finished\n");
        strncpy(response_uri, "/post.html", response_uri_len);
}

MyHTTPServer::MyHTTPServer() {
    this->start();
}

MyHTTPServer::~MyHTTPServer() {}

void MyHTTPServer::start() {

    // Update SSI tags to match exactly what's in the HTML
    static const char *ssi_tags[] = {
        "Key1",     // <!--#Key1-->
        "Value1",   // <!--#Value1--> 
        "Key2",     // <!--#Key2-->
        "Value2"    // <!--#Value2-->
    };

    http_set_ssi_handler(ssi_handler, ssi_tags, 
                        sizeof(ssi_tags) / sizeof(ssi_tags[0]));
    printf("SSI handler and tags registered\n");
    
    start_cgi();
    httpd_init();
    printf("HTTPS server started\n");
}

void MyHTTPServer::start_cgi() {
    static const tCGI cgi_handlers[] = {
        {"/", [](int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) -> const char* {
            MyHTTPServer::debugInfo(iIndex, iNumParams, pcParam, pcValue);
            return "/index.html";
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
        }}
    };
    http_set_cgi_handlers(cgi_handlers, sizeof(cgi_handlers) / sizeof(cgi_handlers[0]));
}

void MyHTTPServer::debugInfo(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    printf("Debug info\n");
    printf("Index: %d\n", iIndex);
    printf("NumParams: %d\n", iNumParams);
    for (int i = 0; i < iNumParams; i++) {
        printf("Param[%d]: %s\n", i, pcParam[i]);
        printf("Value[%d]: %s\n", i, pcValue[i]);
    }
    printf("\n");
}