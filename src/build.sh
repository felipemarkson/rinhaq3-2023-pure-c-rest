#!/bin/sh
CFLAGS=""
if [ $# -eq 0 ]
  then
   CFLAGS="-Ofast"
else
   CFLAGS="-Og -ggdb -std=c11 -pedantic -DDEBUG -Wanalyzer-too-complex -Wall -Wswitch-enum -Wextra -Werror"
fi

SQLITE_FLAGS=" \
-Ofast \
-DSQLITE_DQS=0 \
-DSQLITE_THREADSAFE=2 \
-DSQLITE_DEFAULT_MEMSTATUS=0 \
-DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 \
-DSQLITE_LIKE_DOESNT_MATCH_BLOBS \
-DSQLITE_MAX_EXPR_DEPTH=0 \
-DSQLITE_OMIT_DECLTYPE \
-DSQLITE_OMIT_DEPRECATED \
-DSQLITE_OMIT_PROGRESS_CALLBACK \
-DSQLITE_OMIT_SHARED_CACHE \
-DSQLITE_USE_ALLOCA \
-DSQLITE_OMIT_JSON=1 \
"

set -xe
if [ ! -d sqlite3 ]; then
   chmod +x get_sqlite3.sh
   ./get_sqlite3.sh
fi
gcc -c  $SQLITE_FLAGS sqlite3/sqlite3.c
gcc $CFLAGS main.c sqlite3.o -o main -Isqlite  -lmicrohttpd