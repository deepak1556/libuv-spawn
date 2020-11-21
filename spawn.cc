#include <errno.h>
#include <functional>
#include <iostream>
#include <string>
#include <mach/vm_page_size.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "libuv/include/uv.h"

uv_loop_t *loop;
int g_mmap_tag = 240;

// Rounds |x| up to a multiple of |m|, where |m| is a power of 2.
inline uintptr_t RoundUp(uintptr_t x, uintptr_t m)
{
	return (x + m - 1) & ~(m - 1);
}

void* AllocateVM(size_t length, bool use_jit) {
    size_t page_size = vm_page_size;
    size_t allocate_size = RoundUp(length, page_size);
    int fd = VM_MAKE_TAG(g_mmap_tag);
    int access_flags = PROT_READ | PROT_WRITE | PROT_EXEC;
    int map_flags = MAP_ANONYMOUS | MAP_PRIVATE;
    if (use_jit)
        map_flags |= MAP_JIT;
    void* ret = mmap(nullptr, allocate_size, access_flags, map_flags, fd, 0);
    if (ret == MAP_FAILED)
        ret = nullptr;
    return ret;
}

void Spawn(uv_process_t* child_req, char* program, char** args) {
    uint64_t before_run = uv_hrtime();
    int err, status;
    uv_process_options_t options;
    memset(&options, 0, sizeof(uv_process_options_t));

    options.file = program;
    options.args = args;

    uv_run(loop, UV_RUN_DEFAULT);

    int result;
    if ((result = uv_spawn(loop, child_req, &options))) {
        std::cout << uv_strerror(result);
        return;
    } else {
        std::cout << program << " process launched with ID : "
                  << child_req->pid
                  << " and in : " << (uv_hrtime() - before_run) / 1e6 << " ms\n";
    }

    /* verify the child is successfully cleaned up within libuv */
    do {
        err = waitpid(child_req->pid, &status, 0);
    } while (err == -1 && errno == EINTR);

    std::cout << program << " process exited with status : " << status
              << " and in : " << (uv_hrtime() - before_run) / 1e6 << " ms\n\n";

    uv_close((uv_handle_t*) child_req, nullptr);
}

int main(int argc, const char *argv[]) {
    loop = uv_default_loop();
    size_t length = 16553600;
    bool use_jit = false;
    if (argc > 1 && !strcmp(argv[1], "--use-jit")) {
        use_jit = true;
    }

    for (auto i = 0; i < 50; i++) {
        void* result = AllocateVM(length, use_jit);
        if (result == nullptr) {
            std::cout << "Unable to allocate vm of given size\n";
            return 0;
        }
    }

    for (auto i = 0; i < 10; i++) {
        uv_process_t child_req;
        auto** args = new char*[3];
        args[0] = strdup("ls");
        args[1] = strdup("-l");
        args[2] = nullptr;
        Spawn(&child_req, strdup("ls"), args);
    }

    return 0;
}
