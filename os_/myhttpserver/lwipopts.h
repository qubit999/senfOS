#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define NO_SYS                          1
#define LWIP_RAW                        1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        (128 * 1024)
#define MEMP_NUM_TCP_SEG                128
#define MEMP_NUM_SYS_TIMEOUT            20
#define MEMP_MEM_MALLOC                 1
#define MBEDTLS_PLATFORM_MEMORY         1
#define PBUF_POOL_SIZE                  64
#define LWIP_TCP                        1
#define TCP_TTL                         255
#define TCP_QUEUE_OOSEQ                 1
#define TCP_MSS                         1460  // Standard MSS size
#define TCP_SND_BUF                     (16 * TCP_MSS)  // Increased to 16
#define TCP_SND_QUEUELEN                2 * (TCP_SND_BUF / TCP_MSS)
#define TCP_RX_BUF                      (16 * TCP_MSS)  // Increased to 16
#define TCP_WND                         (16 * TCP_MSS)  // Increased to 16
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   6
#define LWIP_TCP_KEEPALIVE              1
#define LWIP_ICMP                       1
#define LWIP_DHCP                       1
#define LWIP_UDP                        1
#define UDP_TTL                         255
#define LWIP_STATS                      0
#define LWIP_PROVIDE_ERRNO              1
#define LWIP_HTTP_CLIENT                1
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
#define LWIP_DNS                        1
#define DNS_MAX_SERVERS                 4
#define LWIP_SO_RCVTIMEO                1
#define LWIP_TCP_KEEPALIVE              1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_IPV6_AUTOCONFIG            1
#define LWIP_IPV6_MLD                   1
#define LWIP_IPV6_FORWARD               1
#define LWIP_DHCP                       1
#define LWIP_AUTOIP                     1
#define LWIP_DHCP_AUTOIP_COOP           1
#define LWIP_IPV6                       1
#define LWIP_IPV6_DNS                   1
#define LWIP_HTTPD_SUPPORT_POST         1
#define LWIP_HTTPD_CGI                  1
#define LWIP_HTTPD_SSI                  1
#define LWIP_HTTPD_SSI_INCLUDE_TAG      1
#define LWIP_HTTPD_SUPPORT_POST         1
#define LWIP_HTTPD_SSI_MULTIPART        1
#define LWIP_HTTPD_SSI_BY_FILE_EXTENSION 1
#define LWIP_HTTPD_DYNAMIC_HEADERS      1
#define HTTPD_FSDATA_FILE               "my_fsdata.c"
#define HTTPD_SERVER_PORT               9999 // CHANGE SERVER PORT HERE
#define HTTPD_SERVER_PORT_HTTPS         443
#define HTTPD_ENABLE_HTTPS              1
#define LWIP_HTTPD_SUPPORT_HTTPS        1

//#define LWIP_DEBUG                      1
#define HTTPD_DEBUG                     LWIP_DBG_ON
//#define DNS_DEBUG                       LWIP_DBG_ON
//#define TCP_DEBUG                       LWIP_DBG_ON
//#define MEM_DEBUG                       LWIP_DBG_ON
//#define PBUF_DEBUG                      LWIP_DBG_ON
//#define MEMP_DEBUG                      LWIP_DBG_ON

#endif // LWIPOPTS_H
