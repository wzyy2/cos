#include <cos/cos.h>
#include <arch/arch.h>

extern uint32_t interrupt_nest;
uint32_t interrupt_from_thread = 0, interrupt_to_thread = 0;
uint32_t thread_switch_interrupt_flag = 0;


/* exception and interrupt handler table */
isr_handler_t isr_table[MAX_HANDLERS];
uint16_t irq_mask_8259A = 0xFFFF;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { {0}, };
struct Pseudodesc idt_pd =
{
    0, sizeof(idt) - 1, (unsigned long) idt,
};

/* exception and interrupt handler table */
extern isr_handler_t isr_table[];
extern isr_handler_t trap_func[];
extern isr_handler_t hdinterrupt_func[];


extern void arch_interrupt_handle(int vector, void *param);

/**
 * @addtogroup x86
 */
/*@{*/


/**
 * this function initializes the interrupt descript table
 *
 */
void arch_idt_init(void)
{	
    extern long Xdefault;
    unsigned int i, j, func;

    for(i=0; i<MAX_HANDLERS; i++)
    {
        isr_table[i] = arch_interrupt_handle;
    }

    // install a default handler
    for (i = 0; i < sizeof(idt)/sizeof(idt[0]); i++)
        SETGATE(idt[i], 0, GD_KT, &Xdefault, 0);

    /*install trap handler*/
    for(i = 0; i < 16; i++)
    {
        func = (int)trap_func[i];
        SETGATE(idt[i], 0, GD_KT, func, 0);
    }

    func = (int)trap_func[3];
    SETGATE(idt[3], 0, GD_KT, func, 3);

    i = 0;

    /*install exteral interrupt handler*/
    for(j = IRQ_OFFSET; j < IRQ_OFFSET + MAX_HANDLERS; j++)
    {
        func = (int)hdinterrupt_func[i];
        SETGATE(idt[j], 0, GD_KT, func, 0);
        i++;
    }

    // Load the IDT
    asm volatile("lidt idt_pd + 2");
}

/**
 * this function will deal with all kinds of kernel trap
 *
 *@param trapno the trap number
 *
 */
extern "C" void arch_trap_irq(int trapno)
{
    arch_interrupt_disable();

    switch(trapno)
    {
    case T_DIVIDE:
        printk("Divide error interrupt\n");
        COS_ASSERT(0);
    case T_PGFLT:
        printk("Page fault interrupt\n");
        COS_ASSERT(0);
    case T_GPFLT:
        printk("General protection interrupt\n");
        COS_ASSERT(0);
    case T_DEFAULT:
        arch_interrupt_handle(T_DEFAULT, NULL);
        return;
    }

    /*kernel bug if run here*/
    COS_ASSERT(0);
}

void arch_interrupt_handle(int vector, void *param)
{
    printk("Unhandled interrupt  occured!!!\n");
    return;
}

/**
 * This function initializes 8259 interrupt controller
 */
static void arch_pic_init()
{
    outb(IO_PIC1, 0x11);
    outb(IO_PIC1+1, IRQ_OFFSET);
    outb(IO_PIC1+1, 1<<IRQ_SLAVE);
    outb(IO_PIC1+1, 0x3);
    outb(IO_PIC1+1, 0xff);
    outb(IO_PIC1, 0x68);
    outb(IO_PIC1, 0x0a);
    outb(IO_PIC2, 0x11);
    outb(IO_PIC2+1, IRQ_OFFSET + 8);
    outb(IO_PIC2+1, IRQ_SLAVE);
    outb(IO_PIC2+1, 0x3);
    outb(IO_PIC2+1, 0xff);
    outb(IO_PIC2, 0x68);
    outb(IO_PIC2, 0x0a);

    if (irq_mask_8259A != 0xFFFF)
    {
        outb(IO_PIC1+1, (char)irq_mask_8259A);
        outb(IO_PIC2+1, (char)(irq_mask_8259A >> 8));
    }

    /* init interrupt nest, and context */
    interrupt_nest = 0;
}

/**
 * This function initializes interrupt descript table and 8259 interrupt controller
 *
 */
void arch_interrupt_init()
{
    arch_idt_init();
    arch_pic_init();
}

void arch_interrupt_umask(int vector)
{
    irq_mask_8259A = irq_mask_8259A&~(1<<vector);
    outb(IO_PIC1+1, (char)irq_mask_8259A);
    outb(IO_PIC2+1, (char)(irq_mask_8259A >> 8));
}

void arch_interrupt_mask(int vector)
{
    irq_mask_8259A = irq_mask_8259A | (1<<vector);
    outb(IO_PIC1+1, (char)irq_mask_8259A);
    outb(IO_PIC2+1, (char)(irq_mask_8259A >> 8));
}

isr_handler_t arch_interrupt_install(int vector, isr_handler_t new_handler, void *param)
{
    isr_handler_t old_handler;
    if(vector < MAX_HANDLERS)
    {
        old_handler = isr_table[vector];
        if (new_handler != NULL) isr_table[vector] = new_handler;
    }

    return old_handler;
}

void arch_interrupt_enable(base_t level)
{
    //if not enable, not enable
    __asm__ __volatile__("pushl %0 ; popfl;":/* no intput */ :"g" (level):"memory", "cc");
}

base_t arch_interrupt_disable()
{
    base_t level;
    __asm__ __volatile__("pushfl ; popl %0 ; cli":"=g" (level):/* no output */  :"memory");
    return level;
}

/*@{*/
