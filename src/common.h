#ifndef SERVER_COMMON
#define SERVER_COMMON
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
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

#endif