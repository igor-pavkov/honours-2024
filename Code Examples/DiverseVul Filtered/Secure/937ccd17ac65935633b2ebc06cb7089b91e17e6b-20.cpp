static void gx_ttfReader__ReleaseGlyph(ttfReader *self, int glyph_index)
{
    gx_ttfReader *r = (gx_ttfReader *)self;

    if (r->extra_glyph_index != glyph_index)
        return;
    r->extra_glyph_index = -1;
    gs_glyph_data_free(&r->glyph_data, "gx_ttfReader__ReleaseExtraGlyph");
}