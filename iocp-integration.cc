#include <errno.h>

#include "libuv/include/uv.h"

uv_loop_t *loop;

int main(int argc, const char *argv[]) {
    loop = uv_default_loop();
    uv_process_t child_req;

    return 0;
}
