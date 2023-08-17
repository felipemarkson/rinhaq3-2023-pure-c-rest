#include "common.h"
#include "handler.c"

#define PORT 8888

int main(void) {
    MHD_AccessHandlerCallback dh = (MHD_AccessHandlerCallback)&handler;
    struct MHD_Daemon* d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                                            dh, NULL, MHD_OPTION_END);
    if (d == NULL) {
        LOG(stderr, "Error on start deamon\n");
        return 1;
    }
    pause();
    MHD_stop_daemon(d);
    return 0;
}
