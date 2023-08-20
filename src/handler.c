#pragma once
#include "common.h"
#include "pessoa.c"
#include "test.h"

#define ROOT_RESPONSE "{\"response\": \"ok\"}"
#define BAD_REQUEST_RESPONSE "{\"response\": \"bad\"}"
#define MSG_ERR "Internal error on line %d: "

typedef enum {
    POST_STAGE_INITIALYZING = 0,
    POST_STAGE_PROCESSING = 1,
    POST_STAGE_RESPONSING = 2,
} Post_stage;

typedef enum {
    RESPONSE_ACCEPTED = MHD_HTTP_ACCEPTED,
    RESPONSE_UNPROCESSABLE = MHD_HTTP_UNPROCESSABLE_CONTENT,
    RESPONSE_INTERNAL_ERROR = MHD_HTTP_INTERNAL_SERVER_ERROR,
} Response;

typedef struct {
    Response http_code;
    Post_stage stage;
} Post;

struct MHD_Response* build_response(char* json, enum MHD_ResponseMemoryMode mode) {
    struct MHD_Response* response =
        MHD_create_response_from_buffer(strlen(json), (void*)json, mode);
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

bool get_pessoas_id(const char* uuid, struct MHD_Response** out_response) {
    char buffer[MAX_JSON_PAYLOAD + 1] = {0};
    strncpy(pessoa_test.id, uuid, UUID_STR_LEN);
    pessoa_as_json(MAX_JSON_PAYLOAD + 1, buffer, &pessoa_test);
    LOG(stdout, "Data send: %s\n", buffer);
    *out_response = build_response(buffer, MHD_RESPMEM_MUST_COPY);
    return true;
}

unsigned int get_handler(const char* url, struct MHD_Response** out_response) {
    bool finded = false;

    if (0 == strcmp(url, "/")) {
        finded = get_root(out_response);
    } else if (strstr(url, "/pessoas/") == url) {
        const char* uuid = &(url[9]);  // the 9 here is related to size of "/pessoas/"
        finded = get_pessoas_id(uuid, out_response);
    } else {
        *out_response = bad_request();
        return MHD_HTTP_BAD_REQUEST;
    }

    if (finded)
        return MHD_HTTP_OK;
    else
        return MHD_HTTP_NOT_FOUND;
}

bool isvalid_post(size_t size, const char* url) {
    if (size < MIN_PESSOA_JSON_SIZE) {
        return false;
    }
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

enum MHD_Result handler(void* cls, struct MHD_Connection* connection, const char* url,
                        const char* method, const char* version,
                        const char* upload_data, size_t* upload_data_size,
                        void** reqptr) {
    struct MHD_Response* response = NULL;
    unsigned int http_code = MHD_HTTP_OK;
    int ret;
    (void)version;
    (void)upload_data;
    (void)upload_data_size;
    (void)cls;

    // GET
    if (0 == strcmp(method, "GET")) {
        // NOTE: Body is ignored.
        LOG(stdout, "GET REQ -> URL:%s | ", url);
        http_code = get_handler(url, &response);
        if (NULL == response) {
            LOG(stderr, "Internal error on %d: No memory\n", __LINE__);
            http_code = RESPONSE_INTERNAL_ERROR;
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
                if (!isvalid_post(data_size, url)) {
                    post->http_code = RESPONSE_UNPROCESSABLE;
                    return MHD_YES;
                }
                LOG(stdout, "POST REQ: URL %s | Data recived: %s\n", url, upload_data);
                Pessoa pessoa = {0};
                enum JSON_PARSER_ERROR ret =
                    json_as_pessoa(upload_data, data_size, false, &pessoa);
                switch (ret) {
                    case JSON_PARSER_OK: {
                        post->http_code = RESPONSE_ACCEPTED;
                        break;
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
                char buffer[MAX_JSON_PAYLOAD] = {0};
                pessoa_as_json(MAX_JSON_PAYLOAD, buffer, &pessoa);
                LOG(stdout, "Data Processed: %s\n", buffer);

                // TODO: process the data.
                post->http_code = RESPONSE_ACCEPTED;
                return MHD_YES;
            }
            case POST_STAGE_RESPONSING: {
                response = build_response_empty();
                ret = MHD_queue_response(connection, post->http_code, response);
                MHD_destroy_response(response);
                free(post);
                *reqptr = NULL;
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