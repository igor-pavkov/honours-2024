static int gx_ttfReader__LoadGlyph(ttfReader *self, int glyph_index, const byte **p, int *size)
{
    gx_ttfReader *r = (gx_ttfReader *)self;
    gs_font_type42 *pfont = r->pfont;
    int code;

    if (r->extra_glyph_index != -1)
        return 0; /* We only maintain a single glyph buffer.
                     It's enough because ttfOutliner__BuildGlyphOutline
                     is optimized for that, and pfont->data.get_outline
                     implements a charstring cache. */
    r->glyph_data.memory = pfont->memory;
    code = pfont->data.get_outline(pfont, (uint)glyph_index, &r->glyph_data);
    r->extra_glyph_index = glyph_index;
    r->pos = 0;
    if (code < 0)
        r->error = code;
    else if (code > 0) {
        /* Should not happen. */
        r->error = gs_note_error(gs_error_unregistered);
    } else {
        *p = r->glyph_data.bits.data;
        *size = r->glyph_data.bits.size;
    }
    return 2; /* found */
}