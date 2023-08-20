#include "common.h"
#include "handler.c"
#include "db.c"

#define PORT 8888
void* g_db = NULL;

void sighandler(int sig) {
    if (sig == SIGINT)
        printf("received SIGINT\n");
    else if (sig == SIGTERM)
        printf("received SIGTERM\n");
    else if (sig == SIGSEGV)
        printf("received SIGSEGV oh... oh...\n");
    fflush(stdout);
    db_close(g_db);
    _exit(sig);
}

int main(void) {
    int sig_fail = 0;
    if (signal(SIGTERM, sighandler) == SIG_ERR)
        sig_fail = SIGTERM;
    else if (signal(SIGINT, sighandler) == SIG_ERR)
        sig_fail = SIGINT;
    else if (signal(SIGSEGV, sighandler) == SIG_ERR)
        sig_fail = SIGSEGV;
    else
        sig_fail = 0;
    if (sig_fail) {
        LOG(stderr, "Error on SIG %d\n", sig_fail);
        return EXIT_FAILURE;
    }

    if (!db_open(&g_db)) return EXIT_FAILURE;
    if (!db_create_tables(&g_db)) return EXIT_FAILURE;
    if (!db_list_tables(&g_db)) return EXIT_FAILURE;

    MHD_AccessHandlerCallback dh = (MHD_AccessHandlerCallback)&handler;
    struct MHD_Daemon* d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                            dh, NULL, MHD_OPTION_END);
    if (d == NULL) {
        LOG(stderr, "Error on start deamon\n");
        return 1;
    }
    (void)getchar();
    LOG(stdout, "Exiting...\n");
    MHD_stop_daemon(d);
    return 0;
}
