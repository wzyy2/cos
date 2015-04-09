#include "cos/runtime.h"
#include "libc.h"

#include <cos/cos.h>
#include <cos/cosHw.h>

void * __dso_handle = 0; 

/*
 *	This function will call the constructors
 */
void runtime_boot_strap()
{
    //Walk and call the constructors in the ctor_list
    
    //the ctor list is defined in the linker script
    extern void (*__CTOR_LIST__)();
    
    //hold current constructor in list
    void (**constructor)() = &__CTOR_LIST__;
    
    //the first int is the number of constructors
    int total = *(int *)constructor;
    
    //increment to first constructor
    constructor++;
    
    while(total)
    {
        (*constructor)();
        total--;
        constructor++;
    }

    libc_system_init("NULL");
}

/*
 *  This function will call the deconstructors
 */
void runtime_exit()
{
    //Walk and call the deconstructors in the dtor_list
    
    //the dtor list is defined in the linker script
    extern void (*__DTOR_LIST__)() ;
    
    //hold current deconstructor in list
    void (**deconstructor)() = &__DTOR_LIST__ ;
    
    //the first int is the number of deconstructors
    int total = *(int *)deconstructor ;
    
    //increment to first deconstructor
    deconstructor++ ;
    
    while(total)
    {
        (*deconstructor)() ;
        total-- ;
        deconstructor++ ;
    }
}



void* operator new(size_t size)
{
	return kmalloc(size);
}

void* operator new[] (size_t size)
{
	return kmalloc(size);
}

void operator delete(void *ptr)
{
	kfree(ptr);
}

void operator delete[] (void *ptr)
{
	kfree(ptr);
}

extern "C" void __pure_virtual()
{
	printk("pure virtual function call\n");
}

extern "C" void abort()
{
	printk("abort()\n");
}

namespace std{
    void terminate()
    {
        printk("terminate()\n");
    }
}
