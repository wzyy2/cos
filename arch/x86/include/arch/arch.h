#ifndef _ARCH_H_
#define _ARCH_H_

#include <asm/asm.h>
#include <arch/grub.h>
#include <arch/systick.h>
#include <arch/interrupt.h>
#include <arch/context.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/*  Timer Register  */
/*******************************************************************/
#define TIMER_CNTR0         (IO_TIMER1 + 0) /* timer 0 counter port */
#define TIMER_CNTR1         (IO_TIMER1 + 1) /* timer 1 counter port */
#define TIMER_CNTR2         (IO_TIMER1 + 2) /* timer 2 counter port */
#define TIMER_MODE          (IO_TIMER1 + 3) /* timer mode port */
#define TIMER_SEL0          0x00    /* select counter 0 */
#define TIMER_SEL1          0x40    /* select counter 1 */
#define TIMER_INTTC         0x00    /* mode 0, intr on terminal cnt */
#define TIMER_ONESHOT       0x02    /* mode 1, one shot */
#define TIMER_RATEGEN       0x04    /* mode 2, rate generator */
#define TIMER_SQWAVE        0x06    /* mode 3, square wave */
#define TIMER_SWSTROBE      0x08    /* mode 4, s/w triggered strobe */
#define TIMER_HWSTROBE      0x0a    /* mode 5, h/w triggered strobe */
#define TIMER_LATCH         0x00    /* latch counter for reading */
#define TIMER_LSB           0x10    /* r/w counter LSB */
#define TIMER_MSB           0x20    /* r/w counter MSB */
#define TIMER_16BIT         0x30    /* r/w counter 16 bits, LSB first */
#define TIMER_BCD           0x01    /* count in BCD */

#define TIMER_FREQ          1193182
#define TIMER_DIV(x)        ((TIMER_FREQ+(x)/2)/(x))

#define IO_TIMER1           0x040   /* 8253 Timer #1 */

/*******************************************************************/
/* Interrupt Controller */
/*******************************************************************/
/* these are processor defined */
#define T_DIVIDE            0       /* divide error */
#define T_DEBUG             1       /* debug exception */
#define T_NMI               2       /* non-maskable interrupt */
#define T_BRKPT             3       /* breakpoint */
#define T_OFLOW             4       /* overflow */
#define T_BOUND             5       /* bounds check */
#define T_ILLOP             6       /* illegal opcode */
#define T_DEVICE            7       /* device not available */
#define T_DBLFLT            8       /* double fault */
/* 9 is reserved */
#define T_TSS               10      /* invalid task switch segment */
#define T_SEGNP             11      /* segment not present */
#define T_STACK             12      /* stack exception */
#define T_GPFLT             13      /* genernal protection fault */
#define T_PGFLT             14      /* page fault */
/* 15 is reserved */
#define T_FPERR             16      /* floating point error */
#define T_ALIGN             17      /* aligment check */
#define T_MCHK              18      /* machine check */
#define T_DEFAULT           500     /* catchall */

#define INTTIMER0           0
#define INTKEYBOARD         1
#define INTUART0_RX         4

/* I/O Addresses of the two 8259A programmable interrupt controllers */
#define IO_PIC1             0x20    /* Master(IRQs 0-7) */
#define IO_PIC2             0xa0    /* Slave(IRQs 8-15) */
#define IRQ_SLAVE           0x2     /* IRQ at which slave connects to master */
#define IRQ_OFFSET          0x20    /* IRQ 0 corresponds to int IRQ_OFFSET */

#define MAX_HANDLERS        16      /*max number of isr handler*/

/*******************************************************************/
/* CRT Register */
/*******************************************************************/
#define MONO_BASE           0x3b4
#define MONO_BUF            0xb0000
#define CGA_BASE            0x3d4
#define CGA_BUF             0xb8000

#define CRT_ROWS            25
#define CRT_COLS            80
#define CRT_SIZE                (CRT_ROWS * CRT_COLS)

/*******************************************************************/
/* Keyboard Register */
/*******************************************************************/
#define KBSTATP             0x64    /* kbd controller status port(I) */
#define KBS_DIB             0x01    /* kbd data in buffer */
#define KBDATAP             0x60    /* kbd data port(I) */

/*******************************************************************/
/* Serial Register */
/*******************************************************************/
/*Serial I/O code */
#define COM1                0x3F8
#define COMSTATUS           5
#define COMDATA             0x01
#define COMREAD             0
#define COMWRITE            0

/* Bits definition of the Line Status Register (LSR)*/
#define DR                  0x01    /* Data Ready */
#define OE                  0x02    /* Overrun Error */
#define PE                  0x04    /* Parity Error */
#define FE                  0x08    /* Framing Error */
#define BI                  0x10    /* Break Interrupt */
#define THRE                0x20    /* Transmitter Holding Register Empty */
#define TEMT                0x40    /* Transmitter Empty */
#define ERFIFO              0x80    /* Error receive Fifo */

uint8_t *arch_stack_init(void *tentry, void *parameter,
    uint8_t *stack_addr, void *texit);

#ifdef __cplusplus
}
#endif

#endif /* _ARCH_H_ */
