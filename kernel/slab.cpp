/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/sysheap.h"

#include <cos/cos.h>
#include <cos/ipc/semaphore.h>


/* some statistical variable */
static size_t used_mem, max_mem;

/*
 * slab allocator implementation
 *
 * A slab allocator reserves a ZONE for each chunk size, then lays the
 * chunks out in an array within the zone.  Allocation and deallocation
 * is nearly instantanious, and fragmentation/overhead losses are limited
 * to a fixed worst-case amount.
 *
 * The downside of this slab implementation is in the chunk size
 * multiplied by the number of zones.  ~80 zones * 128K = 10MB of VM per cpu.
 * In a kernel implementation all this memory will be physical so
 * the zone size is adjusted downward on machines with less physical
 * memory.  The upside is that overhead is bounded... this is the *worst*
 * case overhead.
 *
 * Slab management is done on a per-cpu basis and no locking or mutexes
 * are required, only a critical section.  When one cpu frees memory
 * belonging to another cpu's slab manager an asynchronous IPI message
 * will be queued to execute the operation.   In addition, both the
 * high level slab allocator and the low level zone allocator optimize
 * M_ZERO requests, and the slab allocator does not have to pre initialize
 * the linked list of chunks.
 *
 * XXX Balancing is needed between cpus.  Balance will be handled through
 * asynchronous IPIs primarily by reassigning the z_Cpu ownership of chunks.
 *
 * XXX If we have to allocate a new zone and M_USE_RESERVE is set, use of
 * the new zone should be restricted to M_USE_RESERVE requests only.
 *
 *  Alloc Size  Chunking        Number of zones
 *  0-127       8               16
 *  128-255     16              8
 *  256-511     32              8
 *  512-1023    64              8
 *  1024-2047   128             8
 *  2048-4095   256             8
 *  4096-8191   512             8
 *  8192-16383  1024            8
 *  16384-32767 2048            8
 *  (if CONFIG_MM_PAGE_SIZE is 4K the maximum zone allocation is 16383)
 *
 *  Allocations >= zone_limit go directly to kmem.
 *
 *          API REQUIREMENTS AND SIDE EFFECTS
 *
 *    To operate as a drop-in replacement to the FreeBSD-4.x malloc() we
 *    have remained compatible with the following API requirements:
 *
 *    + small power-of-2 sized allocations are power-of-2 aligned (kern_tty)
 *    + all power-of-2 sized allocations are power-of-2 aligned (twe)
 *    + malloc(0) is allowed and returns non-NULL (ahc driver)
 *    + ability to allocate arbitrarily large chunks of memory
 */

/*
 * Chunk structure for free elements
 */
typedef struct slab_chunk
{
    struct slab_chunk *c_next;
} slab_chunk;

/*
 * The IN-BAND zone header is placed at the beginning of each zone.
 */
typedef struct slab_zone
{
    int32_t  z_magic;        /* magic number for sanity check */
    int32_t  z_nfree;        /* total free chunks / ualloc space in zone */
    int32_t  z_nmax;         /* maximum free chunks */

    struct slab_zone *z_next;   /* zoneary[] link if z_nfree non-zero */
    uint8_t  *z_baseptr;     /* pointer to start of chunk array */

    int32_t  z_uindex;       /* current initial allocation index */
    uint32_t  z_chunksize;    /* chunk size for validation */

    int32_t  z_zoneindex;    /* zone index */
    slab_chunk  *z_freechunk;   /* free chunk list */
} slab_zone;

#define ZALLOC_SLAB_MAGIC       0x51ab51ab
#define ZALLOC_ZONE_LIMIT       (16 * 1024)     /* max slab-managed alloc */
#define ZALLOC_MIN_ZONE_SIZE    (32 * 1024)     /* minimum zone size */
#define ZALLOC_MAX_ZONE_SIZE    (128 * 1024)    /* maximum zone size */
#define NZONES                  72              /* number of zones */
#define ZONE_RELEASE_THRESH     2               /* threshold number of zones */

static slab_zone *zone_array[NZONES];   /* linked list of zones NFree > 0 */
static slab_zone *zone_free;            /* whole zones that have become free */

static unsigned int zone_free_cnt;
static unsigned int zone_size;
static unsigned int zone_limit;
static unsigned int zone_page_cnt;

/*
 * Misc constants.  Note that allocations that are exact multiples of
 * CONFIG_MM_PAGE_SIZE, or exceed the zone limit, fall through to the kmem module.
 */
#define MIN_CHUNK_SIZE      8       /* in bytes */
#define MIN_CHUNK_MASK      (MIN_CHUNK_SIZE - 1)

/*
 * Array of descriptors that describe the contents of each page
 */
#define PAGE_TYPE_FREE      0x00
#define PAGE_TYPE_SMALL     0x01
#define PAGE_TYPE_LARGE     0x02
struct memusage
{
    uint32_t type:2 ;        /* page type */
    uint32_t size:30;        /* pages allocated or offset from zone */
};
static struct memusage *memusage = NULL;
#define btokup(addr)    \
    (&memusage[((uint32_t)(addr) - heap_start) >> CONFIG_MM_PAGE_BITS])

static uint32_t heap_start, heap_end;

/* page allocator */
struct page_head
{
    struct page_head *next;      /* next valid page */
    size_t page;                 /* number of page  */

    /* dummy */
    char dummy[CONFIG_MM_PAGE_SIZE - (sizeof(struct page_head*) + sizeof (size_t))];
};

static struct page_head *page_list;
Semaphore heap_sem("heap", 1, IPC::FLAG_FIFO);

void *page_alloc(size_t npages)
{
    struct page_head *b, *n;
    struct page_head **prev;

    if(npages == 0)
        return NULL;

    /* lock heap */
    if(Runtime::down_flag)
        heap_sem.take(IPC::WAITING_FOREVER);

    for (prev = &page_list; (b = *prev) != NULL; prev = &(b->next))
    {
        if (b->page > npages)
        {
            /* splite pages */
            n       = b + npages;
            n->next = b->next;
            n->page = b->page - npages;
            *prev   = n;
            break;
        }

        if (b->page == npages)
        {
            /* this node fit, remove this node */
            *prev = b->next;
            break;
        }
    }

    /* unlock heap */
    if(Runtime::down_flag)
        heap_sem.release();

    return b;
}

void *get_zeroed_pages(size_t npages)
{
    page_head *ptr = (page_head *) page_alloc(npages);
    void *optr = ptr;
    if(ptr != NULL) {
        bzero(ptr, CONFIG_MM_PAGE_SIZE * npages);
    }
    return optr;
}

void page_free(void *addr, size_t npages)
{
    struct page_head *b, *n;
    struct page_head **prev;

    COS_ASSERT(addr != NULL);
    COS_ASSERT((uint32_t)addr % CONFIG_MM_PAGE_SIZE == 0);
    COS_ASSERT(npages != 0);

    n = (struct page_head *)addr;

    /* lock heap */
    if(Runtime::down_flag)
        heap_sem.take(IPC::WAITING_FOREVER);

    for (prev = &page_list; (b = *prev) != NULL; prev = &(b->next))
    {
        COS_ASSERT(b->page > 0);
        COS_ASSERT(b > n || b + b->page <= n);

        if (b + b->page == n)
        {
            if (b + (b->page += npages) == b->next)
            {
                b->page += b->next->page;
                b->next  = b->next->next;
            }

            goto _return;
        }

        if (b == n + npages)
        {
            n->page = b->page + npages;
            n->next = b->next;
            *prev   = n;

            goto _return;
        }

        if (b > n + npages)
            break;
    }

    n->page = npages;
    n->next = b;
    *prev   = n;

_return:
    /* unlock heap */
    if(Runtime::down_flag)
        heap_sem.release();
}

/*
 * Initialize the page allocator
 */
static void page_init(void *addr, size_t npages)
{
    COS_ASSERT(addr != NULL);
    COS_ASSERT(npages != 0);

    page_list = NULL;
    page_free(addr, npages);
}

/**
 * This function will init system heap
 *
 * @param begin_addr the beginning address of system page
 * @param end_addr the end address of system page
 */
void system_heap_init(void *begin_addr, void *end_addr)
{
    uint32_t limsize, npages;

    COS_DEBUG_NOT_IN_INTERRUPT;

    /* align begin and end addr to page */
    heap_start = WIDTH_ALIGN((uint32_t)begin_addr, CONFIG_MM_PAGE_SIZE);
    heap_end   = WIDTH_ALIGN_DOWN((uint32_t)end_addr, CONFIG_MM_PAGE_SIZE);

    if (heap_start >= heap_end)
    {
        printk("system_heap_init, wrong address[0x%x - 0x%x]\n",
                   (uint32_t)begin_addr, (uint32_t)end_addr);

        return;
    }

    limsize = heap_end - heap_start;
    npages  = limsize / CONFIG_MM_PAGE_SIZE;

    COS_DEBUG_LOG(COS_DEBUG_SLAB, ("heap[0x%x - 0x%x], size 0x%x, 0x%x pages\n",
                                 heap_start, heap_end, limsize, npages));

    /* init pages */
    page_init((void *)heap_start, npages);

    /* calculate zone size */
    zone_size = ZALLOC_MIN_ZONE_SIZE;
    while (zone_size < ZALLOC_MAX_ZONE_SIZE && (zone_size << 1) < (limsize/1024))
        zone_size <<= 1;

    zone_limit = zone_size / 4;
    if (zone_limit > ZALLOC_ZONE_LIMIT)
        zone_limit = ZALLOC_ZONE_LIMIT;

    zone_page_cnt = zone_size / CONFIG_MM_PAGE_SIZE;

    COS_DEBUG_LOG(COS_DEBUG_SLAB, ("zone size 0x%x, zone page count 0x%x\n",
                                 zone_size, zone_page_cnt));

    /* allocate memusage array */
    limsize  = npages * sizeof(struct memusage);
    limsize  = WIDTH_ALIGN(limsize, CONFIG_MM_PAGE_SIZE);
    memusage = (struct memusage *) page_alloc(limsize/CONFIG_MM_PAGE_SIZE);

    COS_DEBUG_LOG(COS_DEBUG_SLAB, ("memusage 0x%x, size 0x%x\n",
                                 (uint32_t)memusage, limsize));
}

/*
 * Calculate the zone index for the allocation request size and set the
 * allocation request size to that particular zone's chunk size.
 */
inline int zoneindex(size_t *bytes)
{
    /* unsigned for shift opt */
    size_t n = (size_t)*bytes;

    if (n < 128)
    {
        *bytes = n = (n + 7) & ~7;

        /* 8 byte chunks, 16 zones */
        return(n / 8 - 1);
    }
    if (n < 256)
    {
        *bytes = n = (n + 15) & ~15;

        return(n / 16 + 7);
    }
    if (n < 8192)
    {
        if (n < 512)
        {
            *bytes = n = (n + 31) & ~31;

            return(n / 32 + 15);
        }
        if (n < 1024)
        {
            *bytes = n = (n + 63) & ~63;

            return(n / 64 + 23);
        }
        if (n < 2048)
        {
            *bytes = n = (n + 127) & ~127;

            return(n / 128 + 31);
        }
        if (n < 4096)
        {
            *bytes = n = (n + 255) & ~255;

            return(n / 256 + 39);
        }
        *bytes = n = (n + 511) & ~511;

        return(n / 512 + 47);
    }
    if (n < 16384)
    {
        *bytes = n = (n + 1023) & ~1023;

        return(n / 1024 + 55);
    }

    printk("Unexpected byte count %d", n);

    return 0;
}


/**
 * This function will allocate a block from system heap memory.
 * - If the nbytes is less than zero,
 * or
 * - If there is no nbytes sized memory valid in system,
 * the NULL is returned.
 *
 * @param size the size of memory to be allocated
 *
 * @return the allocated memory
 */
void *kmalloc(size_t size)
{
    slab_zone *z;
    int32_t zi;
    slab_chunk *chunk;
    struct memusage *kup;

    /* zero size, return NULL */
    if (size == 0)
        return NULL;

    /*
     * Handle large allocations directly.  There should not be very many of
     * these so performance is not a big issue.
     */
    if (size >= zone_limit)
    {
        size = WIDTH_ALIGN(size, CONFIG_MM_PAGE_SIZE);

        chunk = (slab_chunk *) page_alloc(size >> CONFIG_MM_PAGE_BITS);
        if (chunk == NULL)
            return NULL;

        /* set kup */
        kup = btokup(chunk);
        kup->type = PAGE_TYPE_LARGE;
        kup->size = size >> CONFIG_MM_PAGE_BITS;

        COS_DEBUG_LOG(COS_DEBUG_SLAB,
                     ("malloc a large memory 0x%x, page cnt %d, kup %d\n",
                      size,
                      size >> CONFIG_MM_PAGE_BITS,
                      ((uint32_t)chunk - heap_start) >> CONFIG_MM_PAGE_BITS));

        /* lock heap */
        if(Runtime::down_flag)
            heap_sem.take(IPC::WAITING_FOREVER);

        used_mem += size;
        if (used_mem > max_mem)
            max_mem = used_mem;

        goto done;
    }

    /* lock heap */
    if(Runtime::down_flag)
        heap_sem.take(IPC::WAITING_FOREVER);

    /*
     * Attempt to allocate out of an existing zone.  First try the free list,
     * then allocate out of unallocated space.  If we find a good zone move
     * it to the head of the list so later allocations find it quickly
     * (we might have thousands of zones in the list).
     *
     * Note: zoneindex() will panic of size is too large.
     */
    zi = zoneindex(&size);
    COS_ASSERT(zi < NZONES);

    COS_DEBUG_LOG(COS_DEBUG_SLAB, ("try to malloc 0x%x on zone: %d\n", size, zi));

    if ((z = zone_array[zi]) != NULL)
    {
        COS_ASSERT(z->z_nfree > 0);

        /* Remove us from the zone_array[] when we become empty */
        if (--z->z_nfree == 0)
        {
            zone_array[zi] = z->z_next;
            z->z_next = NULL;
        }

        /*
         * No chunks are available but nfree said we had some memory, so
         * it must be available in the never-before-used-memory area
         * governed by uindex.  The consequences are very serious if our zone
         * got corrupted so we use an explicit printk rather then a KASSERT.
         */
        if (z->z_uindex + 1 != z->z_nmax)
        {
            z->z_uindex = z->z_uindex + 1;
            chunk = (slab_chunk *)(z->z_baseptr + z->z_uindex * size);
        }
        else
        {
            /* find on free chunk list */
            chunk = z->z_freechunk;

            /* remove this chunk from list */
            z->z_freechunk = z->z_freechunk->c_next;
        }

        used_mem += z->z_chunksize;
        if (used_mem > max_mem)
            max_mem = used_mem;


        goto done;
    }

    /*
     * If all zones are exhausted we need to allocate a new zone for this
     * index.
     *
     * At least one subsystem, the tty code (see CROUND) expects power-of-2
     * allocations to be power-of-2 aligned.  We maintain compatibility by
     * adjusting the base offset below.
     */
    {
        uint32_t off;

        if ((z = zone_free) != NULL)
        {
            /* remove zone from free zone list */
            zone_free = z->z_next;
            -- zone_free_cnt;
        }
        else
        {
            /* unlock heap, since page allocator will think about lock */
            if(Runtime::down_flag)
                heap_sem.release();

            /* allocate a zone from page */
            z = (slab_zone *) page_alloc(zone_size / CONFIG_MM_PAGE_SIZE);
            if (z == NULL)
                goto fail;

            /* lock heap */
            if(Runtime::down_flag)
                heap_sem.take(IPC::WAITING_FOREVER);

            COS_DEBUG_LOG(COS_DEBUG_SLAB, ("alloc a new zone: 0x%x\n",
                                         (uint32_t)z));

            /* set message usage */
            for (off = 0, kup = btokup(z); off < zone_page_cnt; off ++)
            {
                kup->type = PAGE_TYPE_SMALL;
                kup->size = off;

                kup ++;
            }
        }

        /* clear to zero */
        memset(z, 0, sizeof(slab_zone));

        /* offset of slab zone struct in zone */
        off = sizeof(slab_zone);

        /*
         * Guarentee power-of-2 alignment for power-of-2-sized chunks.
         * Otherwise just 8-byte align the data.
         */
        if ((size | (size - 1)) + 1 == (size << 1))
            off = (off + size - 1) & ~(size - 1);
        else
            off = (off + MIN_CHUNK_MASK) & ~MIN_CHUNK_MASK;

        z->z_magic     = ZALLOC_SLAB_MAGIC;
        z->z_zoneindex = zi;
        z->z_nmax      = (zone_size - off) / size;
        z->z_nfree     = z->z_nmax - 1;
        z->z_baseptr   = (uint8_t *)z + off;
        z->z_uindex    = 0;
        z->z_chunksize = size;

        chunk = (slab_chunk *)(z->z_baseptr + z->z_uindex * size);

        /* link to zone array */
        z->z_next = zone_array[zi];
        zone_array[zi] = z;

        used_mem += z->z_chunksize;
        if (used_mem > max_mem)
            max_mem = used_mem;

    }

done:
    if(Runtime::down_flag)
        heap_sem.release();

    return chunk;

fail:
    if(Runtime::down_flag)
        heap_sem.release();

    return NULL;
}


/**
 * This function will change the size of previously allocated memory block.
 *
 * @param ptr the previously allocated memory block
 * @param size the new size of memory block
 *
 * @return the allocated memory
 */
void *krealloc(void *ptr, size_t size)
{
    void *nptr;
    slab_zone *z;
    struct memusage *kup;

    if (ptr == NULL)
        return kmalloc(size);
    if (size == 0)
    {
        kfree(ptr);

        return NULL;
    }

    /*
     * Get the original allocation's zone.  If the new request winds up
     * using the same chunk size we do not have to do anything.
     */
    kup = btokup((uint32_t)ptr & ~CONFIG_MM_PAGE_MASK);
    if (kup->type == PAGE_TYPE_LARGE)
    {
        size_t osize;

        osize = kup->size << CONFIG_MM_PAGE_BITS;
        if ((nptr = kmalloc(size)) == NULL)
            return NULL;
        memcpy(nptr, ptr, size > osize ? osize : size);
        kfree(ptr);

        return nptr;
    }
    else if (kup->type == PAGE_TYPE_SMALL)
    {
        z = (slab_zone *)(((uint32_t)ptr & ~CONFIG_MM_PAGE_MASK) -
                          kup->size * CONFIG_MM_PAGE_SIZE);
        COS_ASSERT(z->z_magic == ZALLOC_SLAB_MAGIC);

        zoneindex(&size);
        if (z->z_chunksize == size)
            return(ptr); /* same chunk */

        /*
         * Allocate memory for the new request size.  Note that zoneindex has
         * already adjusted the request size to the appropriate chunk size, which
         * should optimize our bcopy().  Then copy and return the new pointer.
         */
        if ((nptr = kmalloc(size)) == NULL)
            return NULL;

        memcpy(nptr, ptr, size > z->z_chunksize ? z->z_chunksize : size);
        kfree(ptr);

        return nptr;
    }

    return NULL;
}

/**
 * This function will contiguously allocate enough space for count objects
 * that are size bytes of memory each and returns a pointer to the allocated
 * memory.
 *
 * The allocated memory is filled with bytes of value zero.
 *
 * @param count number of objects to allocate
 * @param size size of the objects to allocate
 *
 * @return pointer to allocated memory / NULL pointer if there is an error
 */
void *kcalloc(size_t count, size_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = kmalloc(count * size);

    /* zero the memory */
    if (p)
        memset(p, 0, count * size);

    return p;
}

/**
 * This function will release the previous allocated memory block by kmalloc.
 * The released memory block is taken back to system heap.
 *
 * @param ptr the address of memory which will be released
 */
void kfree(void *ptr)
{
    slab_zone *z;
    slab_chunk *chunk;
    struct memusage *kup;

    /* free a NULL pointer */
    if (ptr == NULL)
        return ;

    /* get memory usage */
#if COS_DEBUG_SLAB
    {
        uint32_t addr = ((uint32_t)ptr & ~CONFIG_MM_PAGE_MASK);
        COS_DEBUG_LOG(COS_DEBUG_SLAB,
                     ("free a memory 0x%x and align to 0x%x, kup index %d\n",
                      (uint32_t)ptr,
                      (uint32_t)addr,
                      ((uint32_t)(addr) - heap_start) >> CONFIG_MM_PAGE_BITS));
    }
#endif

    kup = btokup((uint32_t)ptr & ~CONFIG_MM_PAGE_MASK);
    /* release large allocation */
    if (kup->type == PAGE_TYPE_LARGE)
    {
        uint32_t size;

        /* lock heap */
        if(Runtime::down_flag)
            heap_sem.take(IPC::WAITING_FOREVER);
        /* clear page counter */
        size = kup->size;
        kup->size = 0;

        used_mem -= size * CONFIG_MM_PAGE_SIZE;

        if(Runtime::down_flag)
            heap_sem.release();

        COS_DEBUG_LOG(COS_DEBUG_SLAB,
                     ("free large memory block 0x%x, page count %d\n",
                      (uint32_t)ptr, size));

        /* free this page */
        page_free(ptr, size);

        return;
    }

    /* lock heap */
    if(Runtime::down_flag)
        heap_sem.take(IPC::WAITING_FOREVER);

    /* zone case. get out zone. */
    z = (slab_zone *)(((uint32_t)ptr & ~CONFIG_MM_PAGE_MASK) -
                      kup->size * CONFIG_MM_PAGE_SIZE);
    COS_ASSERT(z->z_magic == ZALLOC_SLAB_MAGIC);

    chunk          = (slab_chunk *)ptr;
    chunk->c_next  = z->z_freechunk;
    z->z_freechunk = chunk;

    used_mem -= z->z_chunksize;

    /*
     * Bump the number of free chunks.  If it becomes non-zero the zone
     * must be added back onto the appropriate list.
     */
    if (z->z_nfree++ == 0)
    {
        z->z_next = zone_array[z->z_zoneindex];
        zone_array[z->z_zoneindex] = z;
    }

    /*
     * If the zone becomes totally free, and there are other zones we
     * can allocate from, move this zone to the FreeZones list.  Since
     * this code can be called from an IPI callback, do *NOT* try to mess
     * with kernel_map here.  Hysteresis will be performed at malloc() time.
     */
    if (z->z_nfree == z->z_nmax &&
        (z->z_next || zone_array[z->z_zoneindex] != z))
    {
        slab_zone **pz;

        COS_DEBUG_LOG(COS_DEBUG_SLAB, ("free zone 0x%x\n",
                                     (uint32_t)z, z->z_zoneindex));

        /* remove zone from zone array list */
        for (pz = &zone_array[z->z_zoneindex]; z != *pz; pz = &(*pz)->z_next)
            ;
        *pz = z->z_next;

        /* reset zone */
        z->z_magic = -1;

        /* insert to free zone list */
        z->z_next = zone_free;
        zone_free = z;

        ++ zone_free_cnt;

        /* release zone to page allocator */
        if (zone_free_cnt > ZONE_RELEASE_THRESH)
        {
            register ubase_t i;

            z         = zone_free;
            zone_free = z->z_next;
            -- zone_free_cnt;

            /* set message usage */
            for (i = 0, kup = btokup(z); i < zone_page_cnt; i ++)
            {
                kup->type = PAGE_TYPE_FREE;
                kup->size = 0;
                kup ++;
            }

            /* unlock heap */
            if(Runtime::down_flag)
                heap_sem.release();

            /* release pages */
            page_free(z, zone_size / CONFIG_MM_PAGE_SIZE);

            return;
        }
    }
    /* unlock heap */
    if(Runtime::down_flag)
        heap_sem.release();
}


void memory_info(uint32_t *total,
                    uint32_t *used,
                    uint32_t *max_used)
{
    if (total != NULL)
        *total = heap_end - heap_start;

    if (used  != NULL)
        *used = used_mem;

    if (max_used != NULL)
        *max_used = max_mem;
}


void list_mem(void)
{
    printk("total memory: %d\n", heap_end - heap_start);
    printk("used memory : %d\n", used_mem);
    printk("maximum allocated memory: %d\n", max_mem);
}



