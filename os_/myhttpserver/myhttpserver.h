#ifndef MY_HTTPSERVER_H
#define MY_HTTPSERVER_H

#define PRIVKEY_PASS "mittelscharf!"

#include <string>
#include "oled.h"
// #include "epd_213.h"
#include "mbedtls/ssl.h"

#include "lwip/altcp_tls.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp.h"

struct altcp_tls_config {
  mbedtls_ssl_config *conf;
  mbedtls_x509_crt *cert;
  mbedtls_pk_context *pkey;
  u8_t cert_count;
  u8_t cert_max;
  u8_t pkey_count;
  u8_t pkey_max;
  mbedtls_x509_crt *ca;
#if defined(MBEDTLS_SSL_CACHE_C) && ALTCP_MBEDTLS_USE_SESSION_CACHE
  /** Inter-connection cache for fast connection startup */
  struct mbedtls_ssl_cache_context cache;
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && ALTCP_MBEDTLS_USE_SESSION_TICKETS
  mbedtls_ssl_ticket_context ticket_ctx;
#endif
};

class MyHTTPServer {
public:
    int port;
    MyHTTPServer();
    ~MyHTTPServer();
    void start();
    void start_cgi();
    void httpd_init();
    static void debugInfo(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
    static Pico_OLED_1_3& getOLED() { return oled; }
    static Pico_OLED_1_3 oled;
    // static epd_213& getEPD() { return epd; }
    // static epd_213 epd;
    struct altcp_tls_config *tls_config = NULL;
    //altcp_tls_config *altcp_tls_create_config_server_privkey_cert(const u8_t *privkey, size_t privkey_len, const u8_t *privkey_pass, size_t privkey_pass_len, const u8_t *cert, size_t cert_len);
};

#endif