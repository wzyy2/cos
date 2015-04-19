#include <cos/cos.h>
#include <arch/arch.h>


uint8_t *arch_stack_init(void *tentry, void *parameter,
    uint8_t *stack_addr, void *texit){

}

void arch_context_switch(ubase_t from, ubase_t to){

}

void arch_context_switch_to(ubase_t to){

}

void arch_context_switch_interrupt(ubase_t from, ubase_t to){

}

void arch_interrupt_init(){

}

void arch_interrupt_mask(int vector){

}

void arch_interrupt_umask(int vector){

}

isr_handler_t arch_interrupt_install(int              vector,
                            isr_handler_t handler,
                            void            *param)
{

}

void arch_interrupt_enable(base_t)
{

}

base_t arch_interrupt_disable()
{

}

void arch_tick_init(void)
{
}
