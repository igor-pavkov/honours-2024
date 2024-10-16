void ttfFont__destroy(ttfFont *self, gs_font_dir *dir)
{
    gs_memory_t *mem = dir->memory->stable_memory;

    ttfFont__finit(self);
    gs_free_object(mem, self, "ttfFont__destroy");
    ttfInterpreter__release(&dir->tti);
    gx_san__release(&dir->san);
    if (dir->tti == NULL && dir->ttm != NULL) {
        gs_free_object(mem, dir->ttm, "ttfFont__destroy(gx_ttfMemory)");
        dir->ttm = NULL;
    }
}