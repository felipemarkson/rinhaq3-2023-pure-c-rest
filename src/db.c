#include <sqlite3.h>

#include "common.h"

#define CREATETABLE_PESSOA                            \
    "CREATE TABLE IF NOT EXISTS pessoa ("             \
    "uuid BLOB PRIMARY KEY, "                         \
    "apelido TEXT NOT NULL, "                         \
    "nome TEXT NOT NULL, "                            \
    "nascimento DATE NOT NULL, "                      \
    "uuid_stack BLOB, "                               \
    "FOREIGN KEY (uuid_stack) REFERENCES stack(uuid)" \
    ") WITHOUT ROWID;"

#define CREATETABLE_STACK                \
    "CREATE TABLE IF NOT EXISTS stack (" \
    "uuid BLOB PRIMARY KEY, "            \
    "nome TEXT NOT NULL "                \
    ") WITHOUT ROWID;"

#define LISTTABLES                                                          \
    "SELECT name FROM sqlite_schema WHERE type ='table' AND name NOT LIKE " \
    "'sqlite_\%';"

bool db_open(void **db) {
    sqlite3 *inner_db;
    int rc = sqlite3_open(":memory:", &inner_db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(inner_db));
        sqlite3_close(inner_db);
        return false;
    }
    *db = inner_db;
    return true;
}

void db_close(void **db) { sqlite3_close((sqlite3 *)*db); }

typedef struct {
    /* data */
} Sql_result;

bool db_create_tables(void **db) {
    char *zErrMsg = NULL;
    int ret = sqlite3_exec((sqlite3 *)*db, CREATETABLE_STACK, NULL, NULL, &zErrMsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL error on creating table 'stack': %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }

    ret = sqlite3_exec((sqlite3 *)*db, CREATETABLE_PESSOA, NULL, NULL, &zErrMsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL error on creating table 'pessoa': %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}

int db_list_tables_callback(void *ptr, int n_rows, char **rows, char **cols_names) {
    int *count = ptr;
    (void)cols_names;
    if (*count == 0) {
        printf("TABLE NAMES\n-----------\n");
    }
    *count += 1;
    for (int i = 0; i < n_rows; i++) {
        printf("%s\n", rows[i]);
    }
    return 0;
}

bool db_list_tables(void **db) {
    char *zErrMsg = NULL;
    int count = 0;
    int ret = sqlite3_exec((sqlite3 *)*db, LISTTABLES, &db_list_tables_callback, &count,
                           &zErrMsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL error on creating table 'stack': %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}
