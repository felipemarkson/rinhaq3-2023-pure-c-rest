#pragma once
#include "common.h"
#include "pessoa.c"
#include "sqlite3/sqlite3.h"

#define CREATETABLE_PESSOA                \
    "CREATE TABLE IF NOT EXISTS pessoa (" \
    "uuid BLOB PRIMARY KEY, "             \
    "apelido TEXT UNIQUE NOT NULL, "      \
    "nome TEXT NOT NULL, "                \
    "nascimento DATE NOT NULL "           \
    ") WITHOUT ROWID;"

#define CREATETABLE_STACK                               \
    "CREATE TABLE IF NOT EXISTS relation ("             \
    "pessoa_uuid BLOB, "                                \
    "stack TEXT NOT NULL, "                             \
    "FOREIGN KEY (pessoa_uuid) REFERENCES pessoa(uuid)" \
    ");"

#define LISTTABLES                                                          \
    "SELECT name FROM sqlite_schema WHERE type ='table' AND name NOT LIKE " \
    "'sqlite_%%';"

bool db_open(void **db) {
    sqlite3 *inner_db;
    int rc = sqlite3_open(DB_ADDR, &inner_db);
    if (rc) {
        LOG(stderr, "Can't open database: %s\n", sqlite3_errmsg(inner_db));
        fflush(stderr);
        sqlite3_close(inner_db);
        return false;
    }
    *db = inner_db;
    return true;
}

void db_close(void **db) {
    sqlite3_close((sqlite3 *)*db);
    *db = NULL;
}

bool db_create_tables(void **db) {
    char *zErrMsg = NULL;
    int ret;
    ret = sqlite3_exec((sqlite3 *)*db, CREATETABLE_PESSOA, NULL, NULL, &zErrMsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL error on creating table 'pessoa': %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }

    ret = sqlite3_exec((sqlite3 *)*db, CREATETABLE_STACK, NULL, NULL, &zErrMsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL error on creating table 'stack': %s\n", zErrMsg);
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
    LOG(stdout, "Listing tables\n");
    int ret = sqlite3_exec((sqlite3 *)*db, LISTTABLES, &db_list_tables_callback, &count,
                           &zErrMsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fflush(stderr);
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}

bool db_inner_insert_stack(void **db, const Pessoa *pessoa) {
    sqlite3_stmt *stmt = NULL;
    sqlite3 *inner_db = *db;
    int ret = sqlite3_prepare_v2(inner_db,
                                 "INSERT INTO relation(pessoa_uuid, stack)"
                                 " VALUES(?, ?);",
                                 -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR: %s\n", __LINE__, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }

    for (size_t row = 0; row < MAX_STACK_ITEMS; row++) {
        const char *stack = &GET_ITEM(pessoa->stack, row);
        if (stack[0] == '\0') {
            sqlite3_finalize(stmt);
            return true;
        }
        size_t len = strlen(stack);
        ret = sqlite3_bind_blob(stmt, 1, pessoa->id, sizeof(pessoa->id), SQLITE_STATIC);
        if (ret != SQLITE_OK) {
            LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret,
                sqlite3_errmsg(inner_db));
            sqlite3_finalize(stmt);
            return false;
        }
        ret = sqlite3_bind_text(stmt, 2, stack, len + 1, SQLITE_STATIC);
        if (ret != SQLITE_OK) {
            LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret,
                sqlite3_errmsg(inner_db));
            sqlite3_finalize(stmt);
            return false;
        }
        ret = sqlite3_step(stmt);
        bool ret_ok = (ret == SQLITE_OK) | (ret == SQLITE_ROW) | (ret == SQLITE_DONE);
        if (!ret_ok) {
            LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret,
                sqlite3_errmsg(inner_db));
            sqlite3_finalize(stmt);
            return false;
        }
        ret = sqlite3_reset(stmt);
        if (ret != SQLITE_OK) {
            LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret,
                sqlite3_errmsg(inner_db));
            sqlite3_finalize(stmt);
            return false;
        }
    }
    LOG(stderr, MSG_ERR "Unreachable state", __LINE__);
    sqlite3_finalize(stmt);
    return false;
}

bool db_inner_insert_pessoa(void **db, const Pessoa *pessoa) {
    sqlite3_stmt *stmt = NULL;
    sqlite3 *inner_db = *db;
    int ret = sqlite3_prepare_v2(inner_db,
                                 "INSERT INTO pessoa(uuid, apelido, nome, nascimento)"
                                 " VALUES(?, ?, ?, ?);",
                                 -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR: %s\n", __LINE__, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }
    ret = sqlite3_bind_blob(stmt, 1, pessoa->id, sizeof(pessoa->id), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }
    ret = sqlite3_bind_text(stmt, 2, pessoa->apelido, strlen(pessoa->apelido),
                            SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }
    ret = sqlite3_bind_text(stmt, 3, pessoa->nome, strlen(pessoa->nome), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }

    char buff[NASCIMENTO_STR_LEN + 2] = {0};
    strftime(buff, NASCIMENTO_STR_LEN + 2, "%F", &(pessoa->nascimento));
    ret = sqlite3_bind_text(stmt, 4, buff, strlen(buff), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }

    ret = sqlite3_step(stmt);
    bool ret_ok = (ret == SQLITE_OK) | (ret == SQLITE_ROW) | (ret == SQLITE_DONE);
    if (!ret_ok) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }
    ret = sqlite3_reset(stmt);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool db_insert_pessoa(void **db, const Pessoa *pessoa) {
#ifdef DEBUG
    LOG(stdout, "db.c:%d -> Trying to insert: ", __LINE__);
    print_pessoa(pessoa);
#endif
    bool ret = false;
    ret = db_inner_insert_pessoa(db, pessoa);
    ret = db_inner_insert_stack(db, pessoa);
    if (!ret) return ret;

    return true;
}

enum DB_GET_RESULT {
    DB_GET_OK,
    DB_GET_NOT_FOUND,
    DB_GET_INTERNAL_ERROR,
};

enum DB_GET_RESULT db_get_apelido_exists(void **db, char apelido[static 2]) {
    LOG(stdout, "db.c:%d -> Trying to check pessoa.apelido: %s\n", __LINE__, apelido);

    sqlite3_stmt *stmt = NULL;
    sqlite3 *inner_db = *db;
    int ret = sqlite3_prepare_v2(
        inner_db, "SELECT apelido FROM pessoa WHERE apelido=?;", -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }

    ret = sqlite3_bind_text(stmt, 1, apelido, strlen(apelido), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }

    ret = sqlite3_step(stmt);
    bool ret_ok = (ret == SQLITE_OK) | (ret == SQLITE_ROW) | (ret == SQLITE_DONE);
    if (!ret_ok) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }
    if (ret == SQLITE_ROW) {
        LOG(stdout, "db.c:%d -> %s already exist\n", __LINE__, apelido);
        sqlite3_finalize(stmt);
        return DB_GET_OK;
    } else {
        LOG(stdout, "db.c:%d -> %s not exist\n", __LINE__, apelido);
        sqlite3_finalize(stmt);
        return DB_GET_NOT_FOUND;
    }
}

enum DB_GET_RESULT db_get_pessoa(void **db, Pessoa *pessoa) {
    char buffer[ID_STR_LEN + 1] = {0};
    uuid_unparse(pessoa->id, buffer);
    LOG(stdout, "Trying to get pessoa: %s\n", buffer);

    sqlite3_stmt *stmt = NULL;
    sqlite3 *inner_db = *db;
    int ret = sqlite3_prepare_v2(
        inner_db, "SELECT apelido, nome, nascimento FROM pessoa WHERE uuid=?;", -1,
        &stmt, NULL);

    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR: %s\n", __LINE__, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }
    ret = sqlite3_bind_blob(stmt, 1, pessoa->id, sizeof(pessoa->id), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }

    ret = sqlite3_step(stmt);
    bool ret_ok = (ret == SQLITE_OK) | (ret == SQLITE_ROW) | (ret == SQLITE_DONE);
    if (!ret_ok) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }
    if (ret != SQLITE_ROW) {
        LOG(stdout, "db.c: %d: NOT FOUND!\n", __LINE__);
        sqlite3_finalize(stmt);
        return DB_GET_NOT_FOUND;
    }

    const char *out = (const char *)sqlite3_column_text(stmt, 0);
    int size = sqlite3_column_bytes(stmt, 0);
    snprintf(pessoa->apelido, APELIDO_STR_LEN, "%s", out);

    out = (const char *)sqlite3_column_text(stmt, 1);
    size = sqlite3_column_bytes(stmt, 1);
    snprintf(pessoa->nome, NOME_STR_LEN, "%s", out);

    out = (const char *)sqlite3_column_text(stmt, 2);
    size = sqlite3_column_bytes(stmt, 2);
    char date[NASCIMENTO_STR_LEN + 1] = {0};
    snprintf(date, NASCIMENTO_STR_LEN + 1, "%s", out);
    printf("%s\n", date);
    strptime(date, "%F", &(pessoa->nascimento));
    sqlite3_finalize(stmt);

    // get stacks
    ret = sqlite3_prepare_v2(
        inner_db, "SELECT stack FROM relation WHERE pessoa_uuid=?;", -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR: %s\n", __LINE__, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }
    ret = sqlite3_bind_blob(stmt, 1, pessoa->id, sizeof(pessoa->id), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }
    ret = sqlite3_step(stmt);
    ret_ok = (ret == SQLITE_OK) | (ret == SQLITE_ROW) | (ret == SQLITE_DONE);
    if (!ret_ok) {
        LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret, sqlite3_errmsg(inner_db));
        sqlite3_finalize(stmt);
        return DB_GET_INTERNAL_ERROR;
    }

    size_t row = 0;
    while (ret == SQLITE_ROW) {
        out = (const char *)sqlite3_column_text(stmt, 0);
        size = sqlite3_column_bytes(stmt, 0);
        char *stack_location = &GET_ITEM(pessoa->stack, row);
        strncpy(stack_location, out, size);
        ret = sqlite3_step(stmt);
        ret_ok = (ret == SQLITE_OK) | (ret == SQLITE_ROW) | (ret == SQLITE_DONE);
        if (!ret_ok) {
            LOG(stderr, MSG_ERR "DB_ERR %d: %s\n", __LINE__, ret,
                sqlite3_errmsg(inner_db));
            sqlite3_finalize(stmt);
            return DB_GET_INTERNAL_ERROR;
        }
        ++row;
    }
    sqlite3_finalize(stmt);

    return DB_GET_OK;
}