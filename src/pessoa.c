#pragma once
#include "common.h"
#include "json.h"

#define MIN_PESSOA_JSON_SIZE 70
#define PESSOA_MIN_FIELDS 4

#define PESSOA_TEMPLATE      \
    "{"                      \
    "\"id\":\"%s\","         \
    "\"apelido\":\"%s\","    \
    "\"nome\":\"%s\","       \
    "\"nascimento\":\"%s\"," \
    "\"stack\":[%s]"         \
    "}"

// Assuming buffer is zero initilialized. Output Example: ""C#", "Python", "C++""
void parse_stack_tostr(const char stack[MAX_STACK_SIZE],
                       char buffer[MAX_STACK_LIST_WCOMMA_STR_LEN + 1]) {
    size_t buffer_index = 0;
    size_t stack_index = 0;
    size_t last_len = 0;
    while ((buffer_index < MAX_STACK_LIST_WCOMMA_STR_LEN) &&
           (stack_index < MAX_STACK_ITEMS)) {
        const char* stack_str = &GET_ITEM(stack, stack_index);
        if (stack_str[0] == '\0') break;
        last_len = strlen(stack_str);
        sprintf(&(buffer[buffer_index]), "\"%s\",", stack_str);
        buffer_index += last_len + 3;
        ++stack_index;
    }
    buffer[buffer_index - 1] = '\0';
}

void parse_str_tostack(char buffer[MAX_STACK_LIST_WCOMMA_STR_LEN + 1],
                       char stack[MAX_STACK_SIZE]) {
    const char delimiter[] = "\",";
    char* next_token;
    char* token = strtok_r(buffer, delimiter, &next_token);
    for (size_t i = 0; i < MAX_STACK_ITEMS; i++) {
        if (token == NULL) break;
        char* item = &GET_ITEM(stack, i);
        strncpy(item, token, ITEM_STR_LEN);
        token = strtok_r(NULL, delimiter, &next_token);
    }
}

int pessoa_as_json(size_t buffer_size, char in_out_buffer[static buffer_size],
                   const Pessoa* pessoa) {
    char date[11] = {0};
    char list_str[MAX_STACK_LIST_WCOMMA_STR_LEN + 1] = {0};
    parse_stack_tostr(pessoa->stack, list_str);
    strftime(date, NASCIMENTO_STR_LEN + 1, "%F", &(pessoa->nascimento));

    char uuid_str[ID_STR_LEN + 1] = {0};
    uuid_unparse(pessoa->id, uuid_str);
    return snprintf(in_out_buffer, buffer_size, PESSOA_TEMPLATE, uuid_str,
                    pessoa->apelido, pessoa->nome, date, list_str);
}

enum JSON_PARSER_ERROR {
    JSON_PARSER_INVALID,
    JSON_PARSER_MEM,
    JSON_PARSER_OK,
};

enum JSON_PARSER_ERROR get_field_str(size_t max_len, size_t min_len,
                                     char out[static max_len + 1],
                                     struct json_value_s* field_value) {
    struct json_string_s* str = json_value_as_string(field_value);
    if (str == NULL) return JSON_PARSER_INVALID;
    if (str->string_size > (max_len + 1)) return JSON_PARSER_INVALID;
    if (str->string_size < min_len) return JSON_PARSER_INVALID;
    memcpy(out, str->string, str->string_size);
    return JSON_PARSER_OK;
}

enum JSON_PARSER_ERROR get_field_array_of_str(
    size_t max_array_len, size_t min_array_len, size_t max_str_len,
    char out[static max_array_len * (max_str_len + 1)],
    struct json_value_s* field_value) {
    struct json_array_s* array = (struct json_array_s*)field_value->payload;
    if (array == NULL) return JSON_PARSER_INVALID;
    if (array->length > max_array_len) return JSON_PARSER_INVALID;
    if (array->length < min_array_len) return JSON_PARSER_INVALID;

    struct json_array_element_s* array_elem = array->start;
    if (array_elem == NULL) return JSON_PARSER_INVALID;
    size_t row = 0;
    while (array_elem) {
        if (row > max_array_len) return JSON_PARSER_INVALID;
        struct json_value_s* value = array_elem->value;
        if (value->type != json_type_string) return JSON_PARSER_INVALID;
        int ret = get_field_str(max_str_len, 0,
                                &GET_ITEM_FROM_ARRAY(out, row, max_str_len + 1), value);
        if (ret != JSON_PARSER_OK) return ret;
        array_elem = array_elem->next;
        ++row;
    }
    return JSON_PARSER_OK;
}

// Assuming pessoa = {0};
enum JSON_PARSER_ERROR json_as_pessoa(const char* in, size_t in_size, bool has_id,
                                      Pessoa* pessoa) {
    struct json_value_s* root = json_parse(in, in_size);

    if (root == NULL) return JSON_PARSER_INVALID;
    if (root->type != json_type_object) return JSON_PARSER_INVALID;

    struct json_object_s* object = (struct json_object_s*)root->payload;

    size_t min_itens = has_id ? PESSOA_MIN_FIELDS : PESSOA_MIN_FIELDS - 1;

    enum JSON_PARSER_ERROR ret;

    if (object->length < min_itens) return JSON_PARSER_INVALID;
    struct json_object_element_s* field = object->start;
    if (field == NULL) return JSON_PARSER_INVALID;
    do {
        if (field->name == NULL) return JSON_PARSER_INVALID;
        const char* field_name = field->name->string;
        struct json_value_s* field_value = field->value;

        if (streq(field_name, "apelido")) {
            if (field_value->type == json_type_string) {
                int value =
                    get_field_str(APELIDO_STR_LEN, 1, pessoa->apelido, field_value);
                if (value != JSON_PARSER_OK) return value;
            } else
                return JSON_PARSER_INVALID;
        } else if (streq(field_name, "nome")) {
            if (field_value->type == json_type_string) {
                ret = get_field_str(NOME_STR_LEN, 1, pessoa->nome, field_value);
                if (ret != JSON_PARSER_OK) return ret;
            } else
                return JSON_PARSER_INVALID;
        } else if (streq(field_name, "nascimento")) {
            if (field_value->type == json_type_string) {
                char date[NASCIMENTO_STR_LEN + 1] = {0};
                ret = get_field_str(NASCIMENTO_STR_LEN, NASCIMENTO_STR_LEN, date,
                                    field_value);
                if (ret != JSON_PARSER_OK) return ret;
                char* out = strptime(date, "%F", &(pessoa->nascimento));
                if (out[0] != '\0') return JSON_PARSER_INVALID;
            } else
                return JSON_PARSER_INVALID;
        } else if (streq(field_name, "stack")) {
            if (field_value->type == json_type_array) {
                ret = get_field_array_of_str(MAX_STACK_ITEMS, 0, ITEM_STR_LEN,
                                             pessoa->stack, field_value);
                if (ret != JSON_PARSER_OK) return ret;
            } else if (field_value->type == json_type_null)
                pessoa->stack[0] = '\0';
            else
                return JSON_PARSER_INVALID;
        } else if (streq(field_name, "id")) {
            char uuid[ID_STR_LEN + 1] = {0};
            ret = get_field_str(ID_STR_LEN, ID_STR_LEN, uuid, field_value);
            if (ret != JSON_PARSER_OK) return ret;
            ret = uuid_parse(uuid, pessoa->id);
            if (ret != 0) return JSON_PARSER_INVALID;
        } else {
            return JSON_PARSER_INVALID;
        }

    } while ((field = field->next));

    free(root);
    return JSON_PARSER_OK;
}

// Assuming buffer was zero initialized;
void parse_pessoas_as_list(char buffer[MAX_LIST_PESSOAS_PAYLOAD],
                           const Pessoa pessoas[MAX_DB_PESSOAS]) {
    buffer[0] = '[';
    int buffer_index = 1;
    size_t i = 0;
    for (; i < MAX_DB_PESSOAS; i++) {
        if (pessoas[i].apelido[0] == '\0') break;  // the last person
        buffer_index +=
            pessoa_as_json(MAX_JSON_PAYLOAD, &(buffer[buffer_index]), &(pessoas[i]));
        buffer[buffer_index] = ',';
        ++buffer_index;
    }
    if (i == 0)
        buffer[1] = ']';
    else
        buffer[buffer_index - 1] = ']';  // replace last comma by ]
}

void print_pessoa(const Pessoa* pessoa) {
    char buffer[MAX_JSON_PAYLOAD + 1] = {0};
    pessoa_as_json(MAX_JSON_PAYLOAD, buffer, pessoa);
    LOG(stdout, "Pessoa: %s\n", buffer);
}