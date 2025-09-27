# include "kernel_threads.h"
# include "ports.h"

# define VGA_ADDRESS 0xB8000
# define YELLOW 14
# define RED    4

extern unsigned short *vga_buffer;

void thread_a() {
    static int i = 0;

    int num = i*i;
    vga_buffer[10*80 + 0] = (('0' + (num / 100)) | (YELLOW << 8));
    vga_buffer[10*80 + 1] = (('0' + ((num / 10) % 10)) | (YELLOW << 8));
    vga_buffer[10*80 + 2] = (('0' + (num % 10)) | (YELLOW << 8));
    i++;

    if (i >= 20) kernel_thread_finish();
}

void thread_b() {
    static int i = 0;
    int num = i*i*i;

    vga_buffer[11*80 + 0] = (('0' + (num / 1000)) | (RED << 8));
    vga_buffer[11*80 + 1] = (('0' + (num / 100) % 10) | (RED << 8));
    vga_buffer[11*80 + 2] = (('0' + (num / 10) % 10) | (RED << 8));
    vga_buffer[11*80 + 3] = (('0' + (num % 10)) | (RED << 8));
    i++;

    if (i >= 20) kernel_thread_finish();
}
