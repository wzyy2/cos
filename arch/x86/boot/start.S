/* the magic number for the multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* the flags for the multiboot header.  */
#define MULTIBOOT_HEADER_FLAGS		0x00000003

#define CONFIG_STACKSIZE            8192

/**
 * @addtogroup I386
 */
/*@{*/

.section .init, "ax"

/* the system entry */
.globl _start
_start:


/*@}*/
