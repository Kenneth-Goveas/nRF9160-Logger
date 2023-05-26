#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/tls_credentials.h>

#include <net/rest_client.h>

LOG_MODULE_REGISTER(rest, CONFIG_REST_LOG_LEVEL);

static char _rest_resp[CONFIG_REST_BUF_SIZE];

int rest_get (const char * url, char * payload) {
    int status;

    struct rest_client_req_context req;
    struct rest_client_resp_context resp;

    const char * header_fields [] = {
        "content-type: " CONFIG_REST_CONT_TYPE "\r\n",
        "x-apikey: " CONFIG_REST_API_KEY "\r\n",
        NULL
    };

    req.host = CONFIG_REST_HOST_NAME;
    req.port = CONFIG_REST_PORT_NUM;
    req.url = url;

    req.http_method = HTTP_GET;
    req.header_fields = header_fields;
    req.body = NULL;

    req.connect_socket = REST_CLIENT_SCKT_CONNECT;
    req.keep_alive = false;
    req.timeout_ms = 1000 * CONFIG_REST_REQ_TIMEOUT;

    req.sec_tag = CONFIG_REST_SEC_TAG;
    req.tls_peer_verify = TLS_PEER_VERIFY_NONE;

    req.resp_buff = _rest_resp;
    req.resp_buff_len = sizeof(_rest_resp);

    memset(_rest_resp, 0, sizeof(_rest_resp));

    LOG_INF("Making GET request");

    status = rest_client_request(&req, &resp);
    if (status < 0) {
        LOG_ERR(
            "Failed to make GET request (%s)",
            strerror(-status)
        );
        return -1;
    }

    if (resp.http_status_code != 200) {
        LOG_ERR(
            "GET request failed (Response code %hu)",
            resp.http_status_code
        );
        return -1;
    }

    strncpy(payload, resp.response, resp.response_len);

    return 0;
}

int rest_put (const char * url, const char * payload) {
    int status;

    struct rest_client_req_context req;
    struct rest_client_resp_context resp;

    const char * header_fields [] = {
        "content-type: " CONFIG_REST_CONT_TYPE "\r\n",
        "x-apikey: " CONFIG_REST_API_KEY "\r\n",
        NULL
    };

    req.host = CONFIG_REST_HOST_NAME;
    req.port = CONFIG_REST_PORT_NUM;
    req.url = url;

    req.http_method = HTTP_PUT;
    req.header_fields = header_fields;
    req.body = payload;

    req.connect_socket = REST_CLIENT_SCKT_CONNECT;
    req.keep_alive = false;
    req.timeout_ms = 1000 * CONFIG_REST_REQ_TIMEOUT;

    req.sec_tag = CONFIG_REST_SEC_TAG;
    req.tls_peer_verify = TLS_PEER_VERIFY_NONE;

    req.resp_buff = _rest_resp;
    req.resp_buff_len = sizeof(_rest_resp);

    memset(_rest_resp, 0, sizeof(_rest_resp));

    LOG_INF("Making PUT request");

    status = rest_client_request(&req, &resp);
    if (status < 0) {
        LOG_ERR(
            "Failed to make PUT request (%s)",
            strerror(-status)
        );
        return -1;
    }

    if (resp.http_status_code != 200) {
        LOG_ERR(
            "PUT request failed (Response code %hu)",
            resp.http_status_code
        );
        return -1;
    }

    return 0;
}

int rest_post (const char * url, const char * payload) {
    int status;

    struct rest_client_req_context req;
    struct rest_client_resp_context resp;

    const char * header_fields [] = {
        "content-type: " CONFIG_REST_CONT_TYPE "\r\n",
        "x-apikey: " CONFIG_REST_API_KEY "\r\n",
        NULL
    };

    req.host = CONFIG_REST_HOST_NAME;
    req.port = CONFIG_REST_PORT_NUM;
    req.url = url;

    req.http_method = HTTP_POST;
    req.header_fields = header_fields;
    req.body = payload;

    req.connect_socket = REST_CLIENT_SCKT_CONNECT;
    req.keep_alive = false;
    req.timeout_ms = 1000 * CONFIG_REST_REQ_TIMEOUT;

    req.sec_tag = CONFIG_REST_SEC_TAG;
    req.tls_peer_verify = TLS_PEER_VERIFY_NONE;

    req.resp_buff = _rest_resp;
    req.resp_buff_len = sizeof(_rest_resp);

    memset(_rest_resp, 0, sizeof(_rest_resp));

    LOG_INF("Making POST request");

    status = rest_client_request(&req, &resp);
    if (status < 0) {
        LOG_ERR(
            "Failed to make POST request (%s)",
            strerror(-status)
        );
        return -1;
    }

    if (resp.http_status_code != 201) {
        LOG_ERR(
            "POST request failed (Response code %hu)",
            resp.http_status_code
        );
        return -1;
    }

    return 0;
}

int rest_delete (const char * url) {
    int status;

    struct rest_client_req_context req;
    struct rest_client_resp_context resp;

    const char * header_fields [] = {
        "content-type: " CONFIG_REST_CONT_TYPE "\r\n",
        "x-apikey: " CONFIG_REST_API_KEY "\r\n",
        NULL
    };

    req.host = CONFIG_REST_HOST_NAME;
    req.port = CONFIG_REST_PORT_NUM;
    req.url = url;

    req.http_method = HTTP_DELETE;
    req.header_fields = header_fields;
    req.body = NULL;

    req.connect_socket = REST_CLIENT_SCKT_CONNECT;
    req.keep_alive = false;
    req.timeout_ms = 1000 * CONFIG_REST_REQ_TIMEOUT;

    req.sec_tag = CONFIG_REST_SEC_TAG;
    req.tls_peer_verify = TLS_PEER_VERIFY_NONE;

    req.resp_buff = _rest_resp;
    req.resp_buff_len = sizeof(_rest_resp);

    memset(_rest_resp, 0, sizeof(_rest_resp));

    LOG_INF("Making DELETE request");

    status = rest_client_request(&req, &resp);
    if (status < 0) {
        LOG_ERR(
            "Failed to make DELETE request (%s)",
            strerror(-status)
        );
        return -1;
    }

    if (resp.http_status_code != 200) {
        LOG_ERR(
            "DELETE request failed (Response code %hu)",
            resp.http_status_code
        );
        return -1;
    }

    return 0;
}
