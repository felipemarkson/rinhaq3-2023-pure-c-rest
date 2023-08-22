#!/bin/sh

set -e

cd ./src && \
./build.sh -DEBUG && \
# cp -p db.sqlite.base db.sqlite
./main
cd ..