#include "common.h"

#define MIN_PESSOA_JSON_SIZE 70
#define PESSOA_TEMPLATE      \
    "{"                      \
    "\"id\":\"%s\","         \
    "\"apelido\":\"%s\","    \
    "\"nome\":\"%s\","       \
    "\"nascimento\":\"%s\"," \
    "\"stack\":[%s]"         \
    "}"

typedef struct {
    char* id;
    char* apelido;
    char* nome;
    char* nascimento;
    char* stack;
} Pessoa_str;

void pessoa_as_json(char* in_out_buffer, size_t buffer_size, const Pessoa_str* pessoa) {
    snprintf(in_out_buffer, buffer_size, PESSOA_TEMPLATE, pessoa->id, pessoa->apelido,
             pessoa->nome, pessoa->nascimento, pessoa->stack);
}