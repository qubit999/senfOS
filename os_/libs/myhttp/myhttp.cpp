#include "myhttp.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <cstring>

MyHTTP::MyHTTP() : m_request_complete(false) {}

MyHTTP::~MyHTTP() {}

std::string MyHTTP::http_get(const std::string& domain, const int port, const std::string& parameters) {
    m_response.clear();
    m_request_complete = false;

    std::string uri = "/" + parameters;
    httpc_connection_t settings;
    memset(&settings, 0, sizeof(settings));
    settings.result_fn = result_callback;

    httpc_state_t* connection;
    err_t err = httpc_get_file_dns(domain.c_str(), port, uri.c_str(), &settings, recv_callback, this, &connection);

    if (err != ERR_OK) {
        return "Error initiating HTTP GET request";
    }

    while (!m_request_complete) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    printf("Response: %s\n", m_response.c_str());
    return m_response;
}

err_t MyHTTP::recv_callback(void* arg, struct altcp_pcb* pcb, struct pbuf* p, err_t err) {
    MyHTTP* http = static_cast<MyHTTP*>(arg);

    if (p == nullptr) {
        http->m_request_complete = true;
        return ERR_OK;
    }

    if (p->tot_len > 0) {
        http->m_response.append(static_cast<char*>(p->payload), p->tot_len);
    }

    altcp_recved(pcb, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}

void MyHTTP::result_callback(void* arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    MyHTTP* http = static_cast<MyHTTP*>(arg);
    http->m_request_complete = true;
}