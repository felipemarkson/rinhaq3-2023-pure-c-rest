#ifndef SERVER_COMMON
#define SERVER_COMMON
#define _XOPEN_SOURCE
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

#ifdef DEBUG
#define LOG(...) fprintf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define MAX_JSON_PAYLOAD (3 * 1024)

#define UUID_STR_LEN 36
#define APELIDO_STR_LEN 32
#define NOME_STR_LEN 100
#define NASCIMENTO_STR_LEN 10
#define ITEM_STR_LEN 32
#define MAX_STACK_ITEMS 50
#define MAX_STACK_LIST_WCOMMA_STR_LEN \
    ((MAX_STACK_ITEMS * ITEM_STR_LEN) + MAX_STACK_ITEMS * 3)

#define MAX_STACK_SIZE (MAX_STACK_ITEMS * (ITEM_STR_LEN + 1))

#define GET_ITEM_FROM_ARRAY(array, row, size_item) ((array)[(row) * (size_item)])

#define GET_ITEM(stack, row) GET_ITEM_FROM_ARRAY((stack), (row), (ITEM_STR_LEN + 1))

typedef struct Pessoa {
    char id[UUID_STR_LEN + 1];
    char apelido[APELIDO_STR_LEN + 1];
    char nome[NOME_STR_LEN + 1];
    struct tm nascimento;
    char stack[MAX_STACK_SIZE];  // 50 items of 32 chars;
} Pessoa;

bool streq(const char* str1, const char* str2) { return (0 == strcmp(str1, str2)); }

#endif