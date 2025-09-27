# ifndef KERNEL_THREADS_H
# define KERNEL_THREADS_H

typedef void (*thread_func)();

int register_thread(thread_func func, int priority);
void kernel_thread_finish();

# endif
