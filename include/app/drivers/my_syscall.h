#include <zephyr/syscall.h>

__syscall int my_syscall(int parameter);

#include <syscalls/my_syscall.h>  // ✅ Include generated syscall wrapper