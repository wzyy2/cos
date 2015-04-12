#ifndef CONTEXT
#define CONTEXT

#include <cos/cosConf.h>
#include <cos/cosDef.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Context interfaces
 */
void arch_context_switch(ubase_t from, ubase_t to);
void arch_context_switch_to(ubase_t to);
void arch_context_switch_interrupt(ubase_t from, ubase_t to);
#ifdef __cplusplus
}
#endif
#endif // CONTEXT

