#include "cos/init.h"

#include <cos/cos.h>

extern init_fn_t __cos_init_start;
extern init_fn_t __cos_init_end;


/**
 * Export function runtime Initialization
 */
void export_runtime_init(void)
{
    const init_fn_t *fn_ptr;

    for (fn_ptr = &__cos_init_start; fn_ptr < &__cos_init_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }
}
