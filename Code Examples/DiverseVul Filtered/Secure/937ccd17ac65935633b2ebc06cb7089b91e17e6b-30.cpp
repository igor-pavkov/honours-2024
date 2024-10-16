static void *gx_ttfMemory__alloc_struct(ttfMemory *self, const ttfMemoryDescriptor *d,  const char *cname)
{
    gs_memory_t *mem = ((gx_ttfMemory *)self)->memory;

    return mem->procs.alloc_struct(mem, (const gs_memory_struct_type_t *)d, cname);
}