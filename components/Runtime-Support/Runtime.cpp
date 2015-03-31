#include <cos.h>

//
//	This is the main entry point for the kernel
//
void _before_main()
{
    //Walk and call the constructors in the ctor_list
    
    //the ctor list is defined in the linker script
    extern void (*_CTOR_LIST__)() ;
    
    //hold current constructor in list
    void (**constructor)() = &_CTOR_LIST__ ;
    
    //the first int is the number of constructors
    int total = *(int *)constructor ;
    
    //increment to first constructor
    constructor++ ;
    
    while(total)
    {
        (*constructor)() ;
        total-- ;
        constructor++ ;
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
	printk("pure virtual function call");
}

extern "C" void abort()
{
	printk("abort()");
}
