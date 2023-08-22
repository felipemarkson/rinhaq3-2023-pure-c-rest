#ifndef SERVER_COMMON
#define SERVER_COMMON
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE
#define __STDC_WANT_LIB_EXT1__ 1
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Includes
#include <microhttpd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#define MSG_ERR "Internal error on line %d: "
#define MAX_DB_TRIES 10
#define MAX_DB_PESSOAS 50
#define DB_ADDR "db/db.sqlite"
#ifdef DEBUG
#define LOG(...) fprintf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define MAX_JSON_PAYLOAD (3 * 1024)
#define MAX_LIST_PESSOAS_PAYLOAD \
    (MAX_DB_PESSOAS * MAX_JSON_PAYLOAD + MAX_JSON_PAYLOAD + 3)

#define ID_STR_LEN 36
#define APELIDO_STR_LEN 32
#define NOME_STR_LEN 100
#define NASCIMENTO_STR_LEN 10
#define ITEM_STR_LEN 32
#define MAX_STACK_ITEMS 52
#define MAX_STACK_LIST_WCOMMA_STR_LEN \
    ((MAX_STACK_ITEMS * ITEM_STR_LEN) + MAX_STACK_ITEMS * 3)

#define MAX_STACK_SIZE (MAX_STACK_ITEMS * (ITEM_STR_LEN + 1))

#define MAX_TERM_LEN (NOME_STR_LEN + 4)

#define GET_ITEM_FROM_ARRAY(array, row, size_item) ((array)[(row) * (size_item)])

#define GET_ITEM(stack, row) GET_ITEM_FROM_ARRAY((stack), (row), (ITEM_STR_LEN + 1))

typedef struct Pessoa {
    uuid_t id;
    char apelido[APELIDO_STR_LEN + 1];
    char nome[NOME_STR_LEN + 1];
    struct tm nascimento;
    char stack[MAX_STACK_SIZE];  // 50 items of 32 chars;
} Pessoa;

bool streq(const char* str1, const char* str2) { return (0 == strcmp(str1, str2)); }
void print_pessoa(const Pessoa* pessoa);

//                                999999999 (MAX USEC)
const struct timespec time2sleep = {0,  10L};  // 10 us

#endif