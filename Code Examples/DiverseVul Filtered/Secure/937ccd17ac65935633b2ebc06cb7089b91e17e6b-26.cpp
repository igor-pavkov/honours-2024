static void gx_ttfReader__Reset(gx_ttfReader *self)
{
    if (self->extra_glyph_index != -1) {
        self->extra_glyph_index = -1;
        gs_glyph_data_free(&self->glyph_data, "gx_ttfReader__Reset");
    }
    self->error = false;
    self->pos = 0;
}