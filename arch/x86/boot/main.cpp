#include <cos.h>
#include <cosHw.h>

#include <libc.h>

//#include "Console.h"
#include "Video.h"

/**
 * @addtogroup x86
 */

 /*@{*/

extern unsigned char __bss_start[];
extern unsigned char __bss_end[];

/* clear .bss */
static void cos_clear_bss(void)
{
    unsigned char *dst;
    dst = __bss_start;
    while (dst < __bss_end)
        *dst++ = 0;
}

extern void* operator new(size_t size);

int main(){
    /* clear .bss */
    cos_clear_bss();

    /* init hardware interrupt */
    arch_interrupt_init();

    libc_system_init("NULL");

    /* init memory system */
    /* RAM 16M */
    system_heap_init((void *)&__bss_end, (void *)(1024UL*1024*8));

    Video *vid = new Video;
    vid->clear();
    vid->write("Hello World!");


    return 0;
}

 /*@{*/

