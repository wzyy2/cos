/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef _OBJECT_H
#define _OBJECT_H

#include <cos/cosDef.h>
#include <cos/cosConf.h>

/**
 * Base structure of Kernel object
 */
class Object
{
public:
    enum object_class_type
    {
        Object_Class_Semaphore, /**< The object is a semaphore. */
        Object_Class_Mutex, /**< The object is a mutex. */
        Object_Class_Event, /**< The object is a event. */
        Object_Class_MailBox,   /**< The object is a mail box. */
        Object_Class_MessageQueue,  /**< The object is a message queue. */
        Object_Class_Timer,  /**< The object is a timer. */
        Object_Class_Device,    /**< The object is a device */
        Object_Class_Thread,    /**< The object is a thread */
        Object_Class_Unknown,   /**< The object is unknown. */
    };

    explicit Object(object_class_type type, const char *name);
    virtual  ~Object();
    const char *name();

    uint16_t get_flag() {
        return flag_;
    };

    std::map<std::string, Object *> get_infomation(object_class_type which){
        return object_container_[which];
    }

protected:
    void detach();
    static Object *find(const char *name, object_class_type type);

    uint16_t flag_ = 0;      /**< flag of kernel object */
    char  name_[CONFIG_NAME_MAX];    /**< name of kernel object */
    object_class_type type_;   /**< type of kernel object */

private:
    static std::map<std::string, Object *> object_container_[Object_Class_Unknown];

};

#endif //_OBJECT_H
