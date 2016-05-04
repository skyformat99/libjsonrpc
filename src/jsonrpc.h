typedef struct _Request Request;

typedef struct Credentials {
    char *username;
    char *password;
} Credential;

Request *request_new(char *method, json_t *params);

void request_free(Request **request);

json_t *jsonrpc_request_new(Request *request);

json_t *send_request(char *url, Credential *cred, Request *request);
