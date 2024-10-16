static bool gx_ttfReader__Eof(ttfReader *self)
{
    gx_ttfReader *r = (gx_ttfReader *)self;

    if (r->extra_glyph_index != -1)
        return r->pos >= r->glyph_data.bits.size;
    /* We can't know whether pfont->data.string_proc has more bytes,
       so we never report Eof for it. */
    return false;
}