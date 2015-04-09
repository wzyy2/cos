#ifndef _OBJECT_H
#define _OBJECT_H

#include <cos/cosDef.h>

/**
 * Base structure of Kernel object
 */
class Object
{
public:
    enum object_class_type
    {
        Object_Class_Device,                             /**< The object is a device */
        Object_Class_Unknown,                            /**< The object is unknown. */
    };

    explicit Object(object_class_type type, const char *name);
    virtual  ~Object();

protected:
    void detach();
    static Object *find(const char *name, object_class_type type);

    std::string       name_;    /**< name of kernel object */
    object_class_type type_;   /**< type of kernel object */

private:
    static std::map<std::string, Object *> object_container_[Object_Class_Unknown];

};

#endif //_OBJECT_H
