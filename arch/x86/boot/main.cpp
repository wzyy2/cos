#include <cos/cos.h>
#include <arch/arch.h>

#include "console.h"
#include "Video.h"


extern unsigned char __bss_start[];
extern unsigned char __bss_end[];

/* clear .bss */
static void clear_bss(void)
{
    unsigned char *dst;
    dst = __bss_start;
    while (dst < __bss_end)
        *dst++ = 0;
}

int main(){
    /* clear .bss */
    clear_bss();

    /* init hardware interrupt */
    arch_interrupt_init();

    /* init memory system */
    /* RAM 32M */
    system_heap_init((void *)&__bss_end, (void *)(1024UL*1024*32));

    /* init the c\c++ runtime environment */
    runtime_boot_strap();

    /* init the console*/
    Console::Instance();
    console_set_device("console");

    /* init systick */
    arch_tick_init();

    /* show system version info*/
    show_version();

    /* export function runtime Initialization*/
    export_runtime_init();

    /* init application */
    //rt_application_init();

    /* init idle thread */
    Idle::init();

    /* start scheduler */
    Scheduler::start();

    while(1);
    /* never reach here */

    /* exit the c\c++ runtime environment */
    runtime_exit();

    return 0;
}

