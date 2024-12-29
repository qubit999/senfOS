#ifndef MY_HTTP_H
#define MY_HTTP_H

#include <string>
#include "lwip/apps/http_client.h"

class MyHTTP {
public:
    MyHTTP();
    ~MyHTTP();

    std::string http_get(const std::string& domain, const int port, const std::string& parameters);

private:
    static err_t recv_callback(void* arg, struct altcp_pcb* pcb, struct pbuf* p, err_t err);
    static void result_callback(void* arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err);

    std::string m_response;
    bool m_request_complete;
};

#endif // MY_HTTP_H