#!/bin/sh

set -e

cd ./src && \
./build.sh DEBUG && \
cd ..
cp -p db.sqlite.base db.sqlite

./src/main