#ifndef _OBJECT_H
#define _OBJECT_H

#include <cos.h>


/**
 * @addtogroup KernelObject
 */

/*@{*/

/**
 * Base structure of Kernel object
 */
class Object
{
public:
    Object();
    virtual  ~Object();

    enum object_class_type
    {
        Object_Class_Thread = 0,                         /**< The object is a thread. */
        Object_Class_Semaphore,                          /**< The object is a semaphore. */
        Object_Class_Mutex,                              /**< The object is a mutex. */
        Object_Class_Event,                              /**< The object is a event. */
        Object_Class_MailBox,                            /**< The object is a mail box. */
        Object_Class_MessageQueue,                       /**< The object is a message queue. */
        Object_Class_MemHeap,                            /**< The object is a memory heap */
        Object_Class_MemPool,                            /**< The object is a memory pool. */
        Object_Class_Device,                             /**< The object is a device */
        Object_Class_Timer,                              /**< The object is a timer. */
        Object_Class_Module,                             /**< The object is a module. */
        Object_Class_Unknown,                            /**< The object is unknown. */
        Object_Class_Static = 0x80                       /**< The object is a static object. */
    };

    void init(Object *object, object_class_type type, char *name);

protected:
    char       name_[16];    /**< name of kernel object */
    uint8_t type_;   /**< type of kernel object */
    uint8_t flag_;   /**< flag of kernel object */
    list_t  list_;   /**< list node of kernel object */

};

/*@}*/

#endif //_OBJECT_H