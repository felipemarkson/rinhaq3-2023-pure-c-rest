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

void pessoa_as_json(size_t buffer_size, char in_out_buffer[static buffer_size],
                    const Pessoa* pessoa) {
    char list_str[MAX_STACK_LIST_WCOMMA_STR_LEN + 1] = {0};
    size_t list_str_index = 0;
    size_t stack_index = 0;
    size_t last_len = 0;
    while ((list_str_index < MAX_STACK_LIST_WCOMMA_STR_LEN) &&
           (stack_index < MAX_STACK_ITEMS)) {
        const char* stack_str = &GET_ITEM(pessoa->stack, stack_index);
        if (stack_str[0] == '\0') break;
        last_len = strlen(stack_str);
        sprintf(&(list_str[list_str_index]), "\"%s\",", stack_str);
        list_str_index += last_len + 3;
        ++stack_index;
    }
    list_str[list_str_index - 1] = '\0';
    char date[11] = {0};
    strftime(date, NASCIMENTO_STR_LEN + 1, "%F", &(pessoa->nascimento));

    char uuid_str[ID_STR_LEN + 1] = {0};
    uuid_unparse(pessoa->id, uuid_str);
    snprintf(in_out_buffer, buffer_size, PESSOA_TEMPLATE, uuid_str, pessoa->apelido,
             pessoa->nome, date, list_str);
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

void print_pessoa(const Pessoa* pessoa) {
    char buffer[MAX_JSON_PAYLOAD + 1] = {0};
    pessoa_as_json(MAX_JSON_PAYLOAD, buffer, pessoa);
    LOG(stdout, "Pessoa: %s\n", buffer);
}