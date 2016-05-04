#include <string.h>

#include <curl/curl.h>
#include <jansson.h>

#include "jsonrpc.h"

#define JSON_RPC_VERSION "2.0"
#define BUFFER_SIZE (256 * 1024) /* 256KB */
#define URL_SIZE 256
#define JSON_RPC_PAYLOAD "{s:s, s:s, s:o, s:i}"

struct _Request {
    char *jsonrpc;
    char *method;
    json_t *params;
    int id;
};

struct curl_result {
    char *data;
    size_t size;
};

Request *request_new(char *method, json_t *params) {
    Request *r = malloc(sizeof(Request));
    if(r == NULL)
        goto err;

    r->jsonrpc = malloc(strlen(JSON_RPC_VERSION) + 1);
    if(r->jsonrpc == NULL)
        goto err;
    strcpy(r->jsonrpc, JSON_RPC_VERSION);

    r->method = malloc(strlen(method) + 1);
    if(r->method == NULL)
        goto err;
    strcpy(r->method, method);

    r->params = params;
    r->id = 0;

    return r;

err:
    if(r)
        free(r);
    return NULL;
}

void request_free(Request **request) {
    free((*request)->jsonrpc);
    free((*request)->method);
    json_decref((*request)->params);
    free(*request);
    *request = NULL;
}

json_t *jsonrpc_request_new(Request *request) {
    json_t *jsonrpc_request;

    jsonrpc_request = json_pack(
        JSON_RPC_PAYLOAD,
        "jsonrpc", request->jsonrpc,
        "method", request->method,
        "params", request->params,
        "id", request->id
    );

    return jsonrpc_request;
}

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb,
                                  void *context) {
    size_t bytec = size * nmemb;
    struct curl_result *mem = (struct curl_result *)context;

    mem->data = realloc(mem->data, mem->size + bytec + 1);
    if(mem->data == NULL) {
        return 0;
    }

    memcpy(&(mem->data[mem->size]), ptr, bytec);
    mem->size += bytec;
    mem->data[mem->size] = 0;

    return nmemb;
}

json_t *send_request(char *url, Credential *cred, Request *request) {
    CURL *curl = NULL;
    CURLcode status;
    struct curl_result chunk;
    chunk.data = malloc(1);
    chunk.size = 0;
    chunk.data[chunk.size] = 0;
    struct curl_slist *headers = NULL;
    char *http_basic_auth;
    long code;
    char *str_payload = NULL;
    json_t *json_response = NULL;
    json_t *payload = NULL;

    payload = jsonrpc_request_new(request);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl)
        goto err;

    curl_easy_setopt(curl, CURLOPT_URL, url);

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    str_payload = json_dumps(payload, 0);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_payload);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    if(cred) {
        asprintf(&http_basic_auth, "%s:%s", cred->username, cred->password);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
        curl_easy_setopt(curl, CURLOPT_USERPWD, http_basic_auth);
    }

    status = curl_easy_perform(curl);
    if(status != CURLE_OK)
        goto err;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if(code != 200) {
        chunk.data[chunk.size] = '\0';
        json_response = json_loads(chunk.data, 0, NULL);
        goto err;
    }

    free(str_payload);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    chunk.data[chunk.size] = '\0';
    json_response = json_loads(chunk.data, 0, NULL);

    return json_response;

err:
    if(chunk.data)
        free(chunk.data);
    if(curl)
        curl_easy_cleanup(curl);
    if(headers)
        curl_slist_free_all(headers);
    if(str_payload)
        free(str_payload);
    curl_global_cleanup();
    return NULL;
}
