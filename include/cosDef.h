#ifndef _COSDEF_H
#define _COSDEF_H

#include <cosConf.h>

#include <stdarg.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup BasicDef
 */

/*@{*/

/* cos version information */
#define C_VERSION                      0L              /**< major version number */
#define C_SUBVERSION                   0L              /**< minor version number */
#define C_REVISION                     1L              /**< revise version number */

/* cos version */
#define COS_VERSION                ((C_VERSION * 10000) + \
                                         (C_VERSION * 100) + C_REVISION)

/* cos basic data type definitions */
typedef signed   char                   int8_t;      /**<  8bit integer type */
typedef signed   short                  int16_t;     /**< 16bit integer type */
typedef signed   long                   int32_t;     /**< 32bit integer type */
typedef unsigned char                   uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned long                   uint32_t;    /**< 32bit unsigned integer type */

/* 32bit CPU */
typedef long                            base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   ubase_t;     /**< Nbit unsigned CPU related data type */

typedef base_t                       err_t;       /**< Type for error number */
typedef uint32_t                     tick_t;      /**< Type for tick count */
typedef base_t                       flag_t;      /**< Type for flags */
typedef ubase_t                      size_t;      /**< Type for size number */
typedef base_t                       off_t;       /**< Type for offset */

//stdio
// typedef ubase_t   (short)            dev_t;       /**< Type for device */  
// typedef uint32_t  (long)             time_t;      /**< Type for time stamp */ 

/*@}*/

/* maximum value of base type */
#define UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#define TICK_MAX                     UINT32_MAX   /**< Maxium number of tick */

/* Compiler Related Definitions */
              

/* va_start */
/* the version of GNU GCC must be greater than 4.x */
// typedef __builtin_va_list   __gnuc_va_list;
// typedef __gnuc_va_list      va_list;
// #define va_start(v,l)       __builtin_va_start(v,l)
// #define va_end(v)           __builtin_va_end(v)
// #define va_arg(v,l)         __builtin_va_arg(v,l)

#define SECTION(x)                  __attribute__((section(x)))
#define UNUSED                      __attribute__((unused))
#define USED                        __attribute__((used))
#define ALIGN(n)                    __attribute__((aligned(n)))
#define WEAK                        __attribute__((weak))


/* initialization export */

typedef int (*init_fn_t)(void);

#define INIT_EXPORT(fn, level)  \
    const init_fn_t __cos_init_##fn SECTION(".cos_init_fn."level) = fn


/* board init routines will be called in board_init() function */
#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, "1")
/* device initialization */
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, "2")
/* components initialization */
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, "3")
/* file system initialization */
#define INIT_FS_EXPORT(fn)              INIT_EXPORT(fn, "4")
/* environment initialization  */
#define INIT_ENV_EXPORT(fn)             INIT_EXPORT(fn, "5")
/* appliation initialization */
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, "6")


/**
 * @addtogroup Error
 */

/*@{*/

/* error code definitions */
#define ERR_OK                          0               /**< There is no error */
#define ERR_ERROR                       1               /**< A generic error happens */
#define ERR_TIMEOUT                     2               /**< Timed out */
#define ERR_FULL                        3               /**< The resource is full */
#define ERR_EMPTY                       4               /**< The resource is empty */
#define ERR_NOMEM                       5               /**< No memory */
#define ERR_NOSYS                       6               /**< No system */
#define ERR_BUSY                        7               /**< Busy */
#define ERR_IO                          8               /**< IO error */

/*@}*/


/**
 * @ingroup BasicDef
 *
 * @def COS_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. COS_ALIGN(13, 4)
 * would return 16.
 */
#define WIDTH_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def COS_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. COS_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define WIDTH_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))


/*@}*/

#ifdef __cplusplus
}
#endif


#endif /* _COSDEF_H */