#include "cos/object.h"

#include <cos/cos.h>
#include <cos/scheduler.h>

std::map<std::string, Object *> Object::object_container_[Object_Class_Unknown];

/**
 * This function will initialize an object and add it to object system
 * management.
 *
 * @param type the object type.
 * @param name the object name. In system, the object's name must be unique.
 */
Object::Object(object_class_type type, const char *name)
{
    register base_t temp;

    /* initialize object's parameters */

    /* set object type*/
    type_ = type;

    /* copy name */
    name_ = name;

    /* lock interrupt */
    temp = arch_interrupt_disable();

    /* insert object into information object container */
    if (object_container_[type].count(name) == 0){
        object_container_[type][name] = this;
    } else {
        printk("ERROR : Object name %s is not unique!\n", name);
        set_errno(-ERR_FULL);
    }

    /* unlock interrupt */
    arch_interrupt_enable(temp);
}

/**
 * This function will delete an object and release object memory.
 *
 */
Object::~Object()
{
    detach();
}

/**
 * This function will detach a static object from object system,
 * and the memory of static object is not freed.
 *
 * @param object the specified object to be detached.
 */
void Object::detach()
{
    register base_t temp;

    /* lock interrupt */
    temp = arch_interrupt_disable();

    /* remove from object container */
    object_container_[type_].erase(name_);

    /* unlock interrupt */
    arch_interrupt_enable(temp);
}


/**
 * This function will find specified name object from object
 * container.
 *
 * @param name the specified name of object.
 * @param type the type of object
 *
 * @return the found object or NULL if there is no this object
 * in object container.
 *
 * @note this function shall not be invoked in interrupt status.
 */
Object *Object::find(const char *name, object_class_type type)
{
    /* parameter check */
    if ((name == NULL) || (type > Object_Class_Unknown))
        return NULL;

    /* which is invoke in interrupt status */
    COS_DEBUG_NOT_IN_INTERRUPT;

    if (object_container_[type].count(name) == 1) {
        return object_container_[type][name];
    }

    return NULL;
}


std::string Object::name()
{
    return name_;
}
