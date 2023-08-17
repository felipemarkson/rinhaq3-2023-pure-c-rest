#!/bin/sh
CFLAGS=""
if [ $# -eq 0 ]
  then
   CFLAGS="-Ofast"
else
   CFLAGS="-Og -ggdb -DDEBUG -fanalyzer -Wanalyzer-too-complex -Wall -Wswitch-enum -Wextra -Werror"
fi
set -xe
gcc $CFLAGS main.c -o main -lmicrohttpd -lsqlite3