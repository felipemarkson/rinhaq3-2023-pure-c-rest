#pragma once
#include "common.h"

Pessoa pessoa_test = {
    .id = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
           'a'},
    .apelido = "NiPvNA7KEtoI80OgtvginZbQhuV68rMN",
    .nome =
        "Tr4QlgF30YxkyALq3fZhojsC1iphwRD4WibpnrWweQv6mBO8j4MQiVgBZGCjfV9F37m4DP2z56n0zE"
        "MLpiTisBFd0WcA9X8wnt5s",
    .nascimento = {.tm_year = (2001 - 1900), .tm_mday = 1, .tm_mon = 0},
    .stack =
        "wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6z"
        "wCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2i"
        "fVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDM"
        "pY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUk"
        "mN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcY"
        "Vn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriD"
        "WUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu"
        "4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwC"
        "vIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifV"
        "driDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY"
        "\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN"
        "6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn"
        "2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWU"
        "DMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4k"
        "UkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvI"
        "cYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdr"
        "iDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0"
        "wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6z"
        "wCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2i"
        "fVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDM"
        "pY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUk"
        "mN6zwCvIcYVn2ifVdriDWUDMpY\0wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\0"};

const char payload_test[] =
    "{\n    \"id\": \"550e8400-e29b-41d4-a716-446655440000\",\n    \"apelido\": "
    "\"NiPvNA7KEtoI80OgtvginZbQhuV68rMN\",\n    \"nome\": "
    "\"Tr4QlgF30YxkyALq3fZhojsC1iphwRD4WibpnrWweQv6mBO8j4MQiVgBZGCjfV9F37m4DP2z56n0zEML"
    "piTisBFd0WcA9X8wnt5s\",\n    \"nascimento\": \"01/01/2001\",\n    \"stack\": [\n  "
    "      \"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\",\n        "
    "\"wu4kUkmN6zwCvIcYVn2ifVdriDWUDMpY\"\n    ]\n}";