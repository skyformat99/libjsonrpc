#include <stdio.h>

#include <jansson.h>

#include "minunit.h"
//#include "../src/jsonrpc.h"
#include "jsonrpc.h"

#define JSON_RPC_VERSION "2.0"
#define METHOD "method.test"

struct _Request {
    char *jsonrpc;
    char *method;
    json_t *params;
    int id;
};

MU_TEST(test_request_new) {
    json_t *params;
    char *method = METHOD;
    char *request_params;
    char *dumped_params;

    params = json_pack(
        "{s:s, s:s}",
        "k1", "v1",
        "k2", "v2"
    );

    Request *request = request_new(method, params);

    mu_assert(
        request != NULL,
        "request_new returned NULL pointer for valid dataset"
    );
    mu_assert(
        strcmp(request->jsonrpc, JSON_RPC_VERSION) == 0,
        "request->jsonrpc is not a valid JSON_RPC_VERSION"
    );

    mu_assert(
        strcmp(request->method, method) == 0,
        "request->method does not match original dataset"
    );
    mu_assert(
        &*request->method != &*method,
        "request->method and passed dataset share a memory address"
    );

    request_params = json_dumps(request->params, JSON_COMPACT);
    dumped_params = json_dumps(params, JSON_COMPACT);
    mu_assert(
        strcmp(request_params, dumped_params) == 0,
        "request->params JSON dump does not match original dataset"
    );
    mu_assert(
        &*request->params == &*params,
        "request->params does not refer to original dataset's json_t struct"
    );

    mu_assert(request->id == 0, "request->id is non-zero");

    request_free(&request);
    mu_check(request == NULL);
}

MU_TEST(test_jsonrpc_request_new) {
    json_t *params;
    char *method = METHOD;
    json_t *jsonrpc_request;
    json_t *json_jsonrpc;
    json_t *json_method;
    json_t *json_params;
    json_t *json_id;

    params = json_pack(
        "{s:s, s:s}",
        "k1", "v1",
        "k2", "v2"
    );

    Request *request = request_new(method, params);

    jsonrpc_request = jsonrpc_request_new(request);

    json_jsonrpc = json_object_get(jsonrpc_request, "jsonrpc");
    mu_check(strcmp(json_string_value(json_jsonrpc), JSON_RPC_VERSION) == 0);

    json_method = json_object_get(jsonrpc_request, "method");
    mu_check(strcmp(json_string_value(json_method), method) == 0);

    json_params = json_object_get(jsonrpc_request, "params");
    mu_check(strcmp(
        json_dumps(json_params, JSON_COMPACT),
        json_dumps(params, JSON_COMPACT)) == 0
    );

    json_id = json_object_get(jsonrpc_request, "id");
    mu_check(json_integer_value(json_id) == 0);
}
