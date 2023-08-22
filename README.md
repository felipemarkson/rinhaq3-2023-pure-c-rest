# Simple REST API in pure C.

_Because I wanted._ 

## Introduction

In third quarter of 2023 [Francisco Zanfranceschi](https://github.com/zanfranceschi) proposes a small tournament to see how "Programming Stacks" will perform in test stress. The rules were simple: An small API (description in PT-BR [here](https://github.com/zanfranceschi/rinha-de-backend-2023-q3/blob/main/INSTRUCOES.md)) in two instances using a persistent database and Nginx, all in Docker images.

At the moment of the tournament, I was learning the _beauty_ of system programming languages like C, C++, Rust, etc. Thus, I decided to implement Francisco's challenge in _pure_ C. It is by itself a huge challenge. However, I also decided to use the _loved_ SQLite as a database. What can be wrong? Right?

Note that, this stress test makes any unadvised (or inexperienced) developer cries in his bed because it simulates the usage of more than 10k users in the majority doing insertions in the database.

Unfortunately, I just had 30 hours to implement it because of my job and family.

I made all the stress tests and it is passing! SQLite is impressive fast, but it was designed to run in a single thread. To overcome this issue, I just add a `while` and a sleep delay waiting for the database to be ready to write/read. It is a silly solution, but it is working.

## API Documentation

This API was **NOT PLANNED** to be used in production.

However, if you are curious to use it, install the dependency [GNU libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/) and run the shell script `runlocal.sh` to compile and run a debug version of the API and `rundocker.sh` to compile and run the "release" version. Any GNU/Linux with a C compiler should work.

The documentation of the endpoints can be found in `API_DOC.md`.

## License

All the source code is licensed under `MIT` license, so be my guest to use this.






