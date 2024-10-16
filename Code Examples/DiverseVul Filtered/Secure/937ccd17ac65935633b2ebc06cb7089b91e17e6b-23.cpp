gx_ttfReader *gx_ttfReader__create(gs_memory_t *mem)
{
    gx_ttfReader *r = gs_alloc_struct(mem, gx_ttfReader, &st_gx_ttfReader, "gx_ttfReader__create");

    if (r != NULL) {
        r->super.Eof = gx_ttfReader__Eof;
        r->super.Read = gx_ttfReader__Read;
        r->super.Seek = gx_ttfReader__Seek;
        r->super.Tell = gx_ttfReader__Tell;
        r->super.Error = gx_ttfReader__Error;
        r->super.LoadGlyph = gx_ttfReader__LoadGlyph;
        r->super.ReleaseGlyph = gx_ttfReader__ReleaseGlyph;
        r->pos = 0;
        r->error = false;
        r->extra_glyph_index = -1;
        memset(&r->glyph_data, 0, sizeof(r->glyph_data));
        r->pfont = NULL;
        r->memory = mem;
        gx_ttfReader__Reset(r);
    }
    return r;
}