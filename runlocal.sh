#!/bin/sh

set -e

cd ./src && \
./build.sh DEBUG && \
cd ..
./src/main