static void *gx_ttfMemory__alloc_bytes(ttfMemory *self, int size,  const char *cname)
{
    gs_memory_t *mem = ((gx_ttfMemory *)self)->memory;

    return gs_alloc_bytes(mem, size, cname);
}