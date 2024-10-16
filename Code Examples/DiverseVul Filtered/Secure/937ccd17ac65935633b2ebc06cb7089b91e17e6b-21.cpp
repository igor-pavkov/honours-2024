static void gx_ttfMemory__free(ttfMemory *self, void *p,  const char *cname)
{
    gs_memory_t *mem = ((gx_ttfMemory *)self)->memory;

    gs_free_object(mem, p, cname);
}