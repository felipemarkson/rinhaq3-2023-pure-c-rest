#pragma once
#include "common.h"
#include "pessoa.c"
#include "test.h"

#define ROOT_RESPONSE "{\"response\": \"ok\"}"
#define BAD_REQUEST_RESPONSE "{\"response\": \"bad\"}"
typedef enum {
    POST_STAGE_INITIALYZING = 0,
    POST_STAGE_PROCESSING = 1,
    POST_STAGE_RESPONSING = 2,
} Post_stage;

typedef enum {
    RESPONSE_CREATED = MHD_HTTP_CREATED,
    RESPONSE_UNPROCESSABLE = MHD_HTTP_UNPROCESSABLE_CONTENT,
    RESPONSE_INTERNAL_ERROR = MHD_HTTP_INTERNAL_SERVER_ERROR,
    RESPONSE_BAD_REQUEST = MHD_HTTP_BAD_REQUEST,
    RESPONSE_NOT_FOUND = MHD_HTTP_NOT_FOUND,
} Response;

typedef struct {
    Response http_code;
    Post_stage stage;
    Pessoa* pessoa;
} Post;

struct MHD_Response* build_response(char* str, enum MHD_ResponseMemoryMode mode) {
    struct MHD_Response* response =
        MHD_create_response_from_buffer(strlen(str), (void*)str, mode);
    if (response == NULL) return NULL;

    MHD_add_response_header(response, "Content-Type", "application/json");
    return response;
}
struct MHD_Response* build_response_empty() {
    struct MHD_Response* response =
        MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    return response;
}

struct MHD_Response* bad_request() {
    return build_response(BAD_REQUEST_RESPONSE, MHD_RESPMEM_PERSISTENT);
}

bool get_root(struct MHD_Response** out_response) {
    *out_response = build_response(ROOT_RESPONSE, MHD_RESPMEM_PERSISTENT);
    return true;
}

// TODO: MAKE RETURN A ERROR IDENTIFIER INSTEAD OF BOOLEAN
bool get_pessoas_id(const char* uuid, struct MHD_Response** out_response) {
    Pessoa pessoa = {0};
    if (uuid_parse(uuid, pessoa.id) != 0) return false;

    void* db = NULL;
    for (size_t count = 0; count < MAX_DB_TRIES; count++) {
        if (db_open(&db)) break;
        nanosleep(&time2sleep, NULL);  // DB_BUSY;
    }
    if (NULL == db) {
        LOG(stderr, "Could not open a DB connection after %d tries\n", MAX_DB_TRIES);
        return false;  // TODO: THIS SHOULD BE INTERNAL ERROR.
    }
    enum DB_GET_RESULT result;
    result = db_get_pessoa(&db, &pessoa);
    db_close(&db);
    if (result != DB_GET_OK) {
        return false;
    }
    char buffer[MAX_JSON_PAYLOAD + 1] = {0};
    pessoa_as_json(MAX_JSON_PAYLOAD + 1, buffer, &pessoa);
    LOG(stdout, "Data send: %s\n", buffer);
    *out_response = build_response(buffer, MHD_RESPMEM_MUST_COPY);
    return true;
}

bool get_pessoas_term(const char* term, struct MHD_Response** out_response) {
    Pessoa pessoas[MAX_DB_PESSOAS] = {0};
    void* db = NULL;
    for (size_t count = 0; count < MAX_DB_TRIES; count++) {
        if (db_open(&db)) break;
        nanosleep(&time2sleep, NULL);  // DB_BUSY;
    }
    if (NULL == db) {
        LOG(stderr, "Could not open a DB connection after %d tries\n", MAX_DB_TRIES);
        return false;
    }

    enum DB_GET_RESULT result;
    result = db_get_pessoas_term(&db, term, pessoas);
    if (result != DB_GET_OK) {
        db_close(&db);
        return false;
    }
    db_close(&db);
    char buffer[MAX_LIST_PESSOAS_PAYLOAD] = {0};

    parse_pessoas_as_list(buffer, pessoas);

    // TODO: insert payload in the response and remove the line above.
    *out_response = build_response(buffer, MHD_RESPMEM_MUST_COPY);

    return true;
}

unsigned int get_handler(struct MHD_Connection* connection, const char* url,
                         struct MHD_Response** out_response) {
    if (streq(url, "/contagem-pessoas")) {
        void* db = NULL;
        char count_pessoas[100] = {0};
        enum DB_GET_RESULT result;
        db_open(&db);
        if (NULL == db) {
            LOG(stderr, "Could not open a DB connection after %d tries\n",
                MAX_DB_TRIES);
            return MHD_HTTP_INTERNAL_SERVER_ERROR;
        }
        result = db_count_pessoas(&db, count_pessoas);
        db_close(&db);
        if (result != DB_GET_OK) {
            return MHD_HTTP_INTERNAL_SERVER_ERROR;
        }
        *out_response = build_response(count_pessoas, MHD_RESPMEM_MUST_COPY);
        return MHD_HTTP_OK;
    }

    if (strstr(url, "/pessoas") != url) {
        *out_response = bad_request();
        return MHD_HTTP_BAD_REQUEST;
    }

    if (strstr(url, "/pessoas/") == url) {
        bool finded = false;
        // The 9 here is related to size of "/pessoas/"
        const char* uuid = (&url[9]);
        finded = get_pessoas_id(uuid, out_response);
        if (finded) return MHD_HTTP_OK;
        *out_response = build_response_empty();
        return MHD_HTTP_NOT_FOUND;
    }

    if (streq(url, "/pessoas")) {
        const char* term =
            MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "t");
        if (term == NULL) {
            *out_response = bad_request();
            return MHD_HTTP_BAD_REQUEST;
        }

        bool ok = get_pessoas_term(term, out_response);
        if (ok) return MHD_HTTP_OK;
        *out_response = build_response_empty();
        return MHD_HTTP_INTERNAL_SERVER_ERROR;
    }

    *out_response = bad_request();
    return MHD_HTTP_BAD_REQUEST;
}

bool isvalid_url(const char* url) {
    bool url_ok = !strcmp(url, "/pessoas") | !strcmp(url, "/pessoas/");
    if (!url_ok) {
        return false;
    }

    return true;
}

unsigned int post_handler(const char* upload_data, struct MHD_Response** out_response) {
    (void)upload_data;
    *out_response = build_response(ROOT_RESPONSE, MHD_RESPMEM_PERSISTENT);
    return MHD_HTTP_OK;
}

static void request_completed(void* cls, struct MHD_Connection* connection,
                              void** reqptr, enum MHD_RequestTerminationCode toe) {
    (void)cls;
    (void)toe;
    (void)connection;
    LOG(stdout, "TERMINATION CODE: %d\n", toe);

    if (NULL == *reqptr) return;  // it is a GET or some error. Nothing to do.
    Post* post = *reqptr;
    Pessoa* pessoa = post->pessoa;
    free(post);
    if (pessoa == NULL) return;
    uuid_clear(pessoa->id);
    free(pessoa);
}

enum MHD_Result handler(void* cls, struct MHD_Connection* connection, const char* url,
                        const char* method, const char* version,
                        const char* upload_data, size_t* upload_data_size,
                        void** reqptr) {
    struct MHD_Response* response = NULL;
    unsigned int http_code = MHD_HTTP_OK;
    int ret;
    (void)version;
    (void)cls;

    // GET
    if (0 == strcmp(method, "GET")) {
        // NOTE: Body is ignored.
        LOG(stdout, "GET REQ -> URL:%s | ", url);
        http_code = get_handler(connection, url, &response);
        if (NULL == response) {
            LOG(stderr, "Internal error on %d: No memory\n", __LINE__);
            http_code = RESPONSE_INTERNAL_ERROR;
            response = build_response_empty();
        }
        ret = MHD_queue_response(connection, http_code, response);
        MHD_destroy_response(response);
        *upload_data_size = 0;
        return ret;
    }

    // POST
    else if (0 == strcmp(method, "POST")) {
        // Check if it is the fist stage.
        if (NULL == *reqptr) {
            Post* post = calloc(sizeof(Post), 1);
            if (NULL == post) {
                LOG(stderr, "Internal error on %d: No memory\n", __LINE__);
                return MHD_NO;
            }
            post->stage = POST_STAGE_INITIALYZING;
            post->http_code = RESPONSE_INTERNAL_ERROR;
            *reqptr = (void*)post;
            return MHD_YES;
        }

        Post* post = *reqptr;
        size_t data_size = *upload_data_size;
        *upload_data_size = 0;  // Zeroing to the next stage.
        post->stage += 1;
        switch (post->stage) {
            case POST_STAGE_INITIALYZING: {
                LOG(stderr, MSG_ERR "Unreachable state\n", __LINE__);
                post->http_code = RESPONSE_INTERNAL_ERROR;
                post->stage = POST_STAGE_PROCESSING;  // Fix the state.
                return MHD_YES;
            }
            case POST_STAGE_PROCESSING: {
                if (!isvalid_url(url)) {
                    post->http_code = RESPONSE_NOT_FOUND;
                    return MHD_YES;
                }
                LOG(stdout, "POST REQ: URL %s | Data recived: %s\n", url, upload_data);
                post->pessoa = calloc(sizeof(Pessoa), 1);
                if (post->pessoa == NULL) {
                    LOG(stderr, MSG_ERR "Memory Error\n", __LINE__);
                    post->http_code = RESPONSE_INTERNAL_ERROR;
                    return MHD_YES;
                }

                enum JSON_PARSER_ERROR ret =
                    json_as_pessoa(upload_data, data_size, false,
                                   post->pessoa);  // TODO: Check if it is bad request

                LOG(stdout, "handler.c:%d Checking if the json can be parsed.\n",
                    __LINE__);
                switch (ret) {
                    case JSON_PARSER_OK: {
                        break;  // we do nothing, just continue.
                    }
                    case JSON_PARSER_INVALID: {
                        post->http_code = RESPONSE_UNPROCESSABLE;
                        return MHD_YES;
                    }
                    case JSON_PARSER_MEM: {
                        post->http_code = RESPONSE_INTERNAL_ERROR;
                        return MHD_YES;
                    }
                    default: {
                        LOG(stderr, MSG_ERR "Unreachable state\n", __LINE__);
                        post->http_code = RESPONSE_INTERNAL_ERROR;
                        return MHD_YES;
                    }
                }

                LOG(stdout, "handler.c:%d Checking if apelido \'%s\' exists.\n",
                    __LINE__, post->pessoa->apelido);
                void* db = NULL;
                if (!db_open(&db)) {
                    post->http_code = RESPONSE_INTERNAL_ERROR;
                    return MHD_YES;
                }

                enum DB_GET_RESULT get_ret =
                    db_get_apelido_exists(&db, post->pessoa->apelido);
                switch (get_ret) {
                    case DB_GET_INTERNAL_ERROR: {
                        db_close(&db);
                        post->http_code = RESPONSE_INTERNAL_ERROR;
                        return MHD_YES;
                    }
                    case DB_GET_OK: {
                        db_close(&db);
                        post->http_code = RESPONSE_UNPROCESSABLE;
                        return MHD_YES;
                    }
                    case DB_GET_NOT_FOUND: {
                        uuid_generate(post->pessoa->id);
                        post->http_code = RESPONSE_CREATED;
                        db_insert_pessoa(&db, post->pessoa);
                        db_close(&db);
                        return MHD_YES;
                    }
                    default: {
                        db_close(&db);
                        LOG(stderr, MSG_ERR "Unreachable state\n", __LINE__);
                        post->http_code = RESPONSE_INTERNAL_ERROR;
                        return MHD_YES;
                    }
                }
            }
            case POST_STAGE_RESPONSING: {
                LOG(stdout, "HTTP RESPONSE: %d\n", post->http_code);
                response = build_response_empty();
                if (post->http_code != RESPONSE_CREATED) {
                    ret = MHD_queue_response(connection, post->http_code, response);
                    MHD_destroy_response(response);
                    return ret;
                }

                char buffer[9 + ID_STR_LEN + 1] = {'/', 'p', 'e', 's', 's',
                                                   'o', 'a', 's', '/'};
                uuid_unparse(post->pessoa->id, &(buffer[9]));
                ret = MHD_add_response_header(response, "Location", buffer);
                if (ret != MHD_YES) {
                    MHD_destroy_response(response);
                    return ret;
                };
                ret = MHD_queue_response(connection, post->http_code, response);
                MHD_destroy_response(response);
                // free(post); // It is done in the request_completed function.
                return ret;
            }
            default: {
                LOG(stderr, MSG_ERR "Unreachable state\n", __LINE__);
                post->stage = POST_STAGE_PROCESSING;  // Fix the state.
                post->http_code = RESPONSE_INTERNAL_ERROR;
                return MHD_YES;
            }
        }
    }

    // Any other HTTP method
    else {
        // NOTE: Body is ignored.
        ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, NULL);
        return ret;
    }
}