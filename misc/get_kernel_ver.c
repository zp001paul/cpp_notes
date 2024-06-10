#include <errno.h>
#include <linux/version.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

int main(int argc, char *argv[]) {

    // method 1: uname() call
    struct utsname name;
    if (uname(&name) == -1) {
        fprintf(stderr, "failed to uname(), errno: %d, error: %s\n", errno,
                strerror(errno));
        return EXIT_FAILURE;
    }
    printf("from uname(), release: %s\n", name.release);

    // method 2: macro in linux/version.h
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
    printf("from macro, 0X%X, %d, %d, %d >= 4.14, we can use RWF_NOWAIT with "
           "aio\n",
           LINUX_VERSION_CODE, LINUX_VERSION_MAJOR, LINUX_VERSION_PATCHLEVEL,
           LINUX_VERSION_SUBLEVEL);
#else
    printf("%s < 4.14, we cannot use RWF_NOWAIT with aio\n", name.release);
#endif

    return EXIT_SUCCESS;
}
