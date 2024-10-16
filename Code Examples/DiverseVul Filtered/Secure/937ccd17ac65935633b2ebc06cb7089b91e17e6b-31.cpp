ttfFont *ttfFont__create(gs_font_dir *dir)
{
    gs_memory_t *mem = dir->memory->stable_memory;
    ttfFont *ttf;

    if (dir->ttm == NULL) {
        gx_ttfMemory *m = gs_alloc_struct(mem, gx_ttfMemory, &st_gx_ttfMemory, "ttfFont__create(gx_ttfMemory)");

        if (!m)
            return 0;
        m->super.alloc_struct = gx_ttfMemory__alloc_struct;
        m->super.alloc_bytes = gx_ttfMemory__alloc_bytes;
        m->super.free = gx_ttfMemory__free;
        m->memory = mem;
        dir->ttm = m;
    }
    if(ttfInterpreter__obtain(&dir->ttm->super, &dir->tti))
        return 0;
    if(gx_san__obtain(mem, &dir->san))
        return 0;
    ttf = gs_alloc_struct(mem, ttfFont, &st_ttfFont, "ttfFont__create");
    if (ttf == NULL)
        return 0;
#ifdef DEBUG
    ttfFont__init(ttf, &dir->ttm->super, DebugRepaint,
                  (gs_debug_c('Y') ? DebugPrint : NULL), mem);
#else
    ttfFont__init(ttf, &dir->ttm->super, DebugRepaint, NULL, mem);
#endif

    return ttf;
}