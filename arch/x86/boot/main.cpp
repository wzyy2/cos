#include <cos/cos.h>
#include <arch/arch.h>

#include "console.h"
#include "Video.h"

extern unsigned long _end;

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

void app_init();

void entry(void *p)
{
    while(1) {
        printk("hello! %d\n", 111);

        Thread::sleep(1000);
    }
}
void entry2(void *p)
{
    //while(1) {
        printk("hello! 222222%d\n", 111);
        //Thread::sleep(1000);
    //}
}
int main(){
    /* clear .bss */
    clear_bss();

    /* init hardware interrupt */
    arch_interrupt_init();

    /* init memory system */
    /* RAM 32M */
    system_heap_init((void *)&__bss_end, (void *)(1024UL*1024*16));

    /* init the c\c++ runtime environment */
    Runtime::boot_strap();

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
    app_init();

    /* init idle thread */
    Idle::init();

    /* start scheduler */
    Scheduler::start();


    while(1);
    /* never reach here */

    /* exit the c\c++ runtime environment */
    Runtime::exit();

    return 0;
}

uint8_t thread_stack[512];



void app_init()
{
    Thread *test = new Thread("test", entry, NULL, &thread_stack[0] \
            , sizeof(thread_stack), Scheduler::THREAD_PRIORITY_MAX - 3, 30);
    test->startup();
}
