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

Semaphore *test_sem;

void entry(void *p)
{
    while(1) {
        printk("hello! %d\n", 111);
        test_sem->take(IPC::WAITING_FOREVER);
        Thread::sleep(2000);
    }
}
void entry2(void *p)
{
    test_sem->take(IPC::WAITING_FOREVER);
    while(1) {
        printk("hello! 222222%d\n", 111);

        Thread::sleep(4000);
    }
}
int main(){
    /* clear .bss */
    clear_bss();

    /* init hardware interrupt */
    arch_interrupt_init();

    /* init memory system */
    /* RAM 32M */
    system_heap_init((void *)&__bss_end, (void *)(1024UL*1024*8));

    /* init the c\c++ runtime environment */
    Runtime::boot_strap();

    test_sem = new Semaphore("test_sem", 1, IPC::FLAG_FIFO);

    /* init the console*/
    Console::Instance();
    console_set_device("console");

    /* show system version info*/
    show_version();

    /* init systick */
    arch_tick_init();

    /* export function runtime Initialization*/
    export_runtime_init();

    /* init application */
    app_init();

    /* init idle thread */
    Idle::init();

    /* start scheduler */
    Scheduler::start();

//    __asm__ __volatile__("movl %1 ; popl %0 ; cli":"=g" (level):/* no output */  :"memory");
    while(1);
    /* never reach here */

    /* exit the c\c++ runtime environment */
    Runtime::exit();

    return 0;
}

ALIGN(4)
uint8_t thread_stack[1024 * 2];
ALIGN(4)
uint8_t thread2_stack[1024 * 2];


void app_init()
{
    Thread *test = new Thread("test", entry, NULL, &thread_stack[0] \
            , sizeof(thread_stack), Scheduler::THREAD_PRIORITY_MAX - 2, 30);
    test->startup();

    Thread *test2 = new Thread("te2st2", entry2, NULL, &thread2_stack[0] \
            , sizeof(thread2_stack), Scheduler::THREAD_PRIORITY_MAX - 3, 30);
    test2->startup();
}
