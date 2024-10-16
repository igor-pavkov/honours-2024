RAMBlock *qemu_ram_alloc_from_ptr(ram_addr_t size, void *host,
                                   MemoryRegion *mr, Error **errp)
{
    return qemu_ram_alloc_internal(size, size, NULL, host, RAM_PREALLOC, mr,
                                   errp);
}