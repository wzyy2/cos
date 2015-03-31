#ifndef _COSHW_H
#define _COSHW_H

#include <cos.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Interrupt handler definition
 */
typedef void (*isr_handler_t)(int vector, void *param);

/*
 * Interrupt interfaces
 */
void arch_interrupt_init();
void arch_interrupt_mask(int vector);
void arch_interrupt_umask(int vector);

isr_handler_t arch_interrupt_install(int              vector,
                                         isr_handler_t handler,
                                         void            *param,
                                         char            *name);

void arch_interrupt_enable(base_t);
base_t arch_interrupt_disable();




#ifdef __cplusplus
}
#endif



#endif /* _COSHW_H */