#include <cos/cos.h>
#include <cos/cosHw.h>
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

const char sasasa[100] = {
    1,2,3,4,5,6,1,2,3,
};


extern "C" void test(){
    printk("test!!!!!!\n");
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




    while(1);

    /* exit the c\c++ runtime environment */
    runtime_exit();


    return 0;
}

