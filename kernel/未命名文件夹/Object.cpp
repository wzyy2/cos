#include "Object.h"


Object::Object()
{

}

Object::~Object()
{

}

void Object::init(Object *object, object_class_type type, char *name)
{
    register base_t temp;
    struct rt_object_information *information;

    /* get object information */
    information = &rt_object_container[type];

    /* initialize object's parameters */

    /* set object type to static */
    object->type = type | RT_Object_Class_Static;

    /* copy name */
    rt_strncpy(object->name, name, RT_NAME_MAX);

    RT_OBJECT_HOOK_CALL(rt_object_attach_hook, (object));

    // /* lock interrupt */
    // temp = rt_hw_interrupt_disable();

    // /* insert object into information object list */
    // rt_list_insert_after(&(information->object_list), &(object->list));

    // /* unlock interrupt */
    // rt_hw_interrupt_enable(temp);

}
