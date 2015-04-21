.globl arch_context_switch
arch_context_switch:
	pushfl					/*pushed eflags*/
/*
 *	add by ssslady@gmail.com 2009-10-14
 *      When we return again the esp should no be change.
 * 	The old code change the esp to esp-4 :-(.
 *	A protection fault maybe occure for img created by some compiler,eg.gcc in the fedor-11
 *      -------------------------------------------------------------------------
 *	entry			old code			new code
 *	EIP	->return esp	EIP				FLAGS ->return esp
 *	...			FLAGS    ->retern esp		CS
 *				CS				EIP
 *				EIP
 */
	popl %eax	/*get flags*/
	popl %ebx	/*get eip*/
	pushl %eax	/*push flags*/
	push %cs	/*push cs*/
	pushl %ebx	/*push eip*/
	
/*-------------------------------------------------------------------
 */

	/*push %cs*/				/*push cs register*/
	/*pushl 0x8(%esp)*/			/*pushed eip register*/

	pushl $0				/*fill irqno*/
	push %ds				/*push ds register*/
	push %es				/*push es register*/
	pushal					/*push eax,ecx,edx,ebx,esp,ebp,esp,edi registers*/
	
	/*movl 0x40(%esp), %eax*/	/*to thread TCB*/
	/*movl 0x3c(%esp), %ebx*/	/*from thread TCB*/
	movl 0x3c(%esp), %eax	/*to thread TCB*/
	movl 0x38(%esp), %ebx	/*from thread TCB*/

	movl %esp, (%ebx)		/*store esp in preempted tasks TCB*/
	movl (%eax), %esp		/*get new task stack pointer*/

	popal					/*restore new task TCB*/
	pop %es
	pop %ds
	add $4,%esp				/*skip irqno*/
	iret

/*
 * void arch_context_switch_to(uint32 to);
 */
.globl arch_context_switch_to
arch_context_switch_to:
	push %ebp
	movl %esp, %ebp

	movl 0x8(%ebp), %eax	/* to thread TCB */
	movl (%eax), %esp		/* get new task stack pointer */

	popal					/* restore new task TCB*/
	pop %es
	pop %ds
	add $4, %esp			/* skip irqno */
	iret

/*
 * void arch_context_switch_interrupt(ubase_t from, ubase_t to);
 */
.globl thread_switch_interrupt_flag
.globl interrupt_from_thread
.globl interrupt_to_thread
.globl arch_context_switch_interrupt
arch_context_switch_interrupt:
	pushl %ebp
	movl %esp, %ebp
	movl 0xc(%ebp), %eax
	movl 0x8(%ebp), %ebx
	
        movl $thread_switch_interrupt_flag, %ecx
	movl (%ecx), %edx
	cmp $0x1, %edx
	jz _reswitch
	
        movl $0x1, %edx				/*set thread_switch_interrupt_flag to 1*/
	movl %edx, (%ecx)
        movl $interrupt_from_thread, %edx	/*set interrupt_from_thread*/
	movl %ebx, (%edx)
_reswitch:
        movl $interrupt_to_thread, %edx		/*set interrupt_to_thread*/
	movl %eax, (%edx)
	leave
	ret
