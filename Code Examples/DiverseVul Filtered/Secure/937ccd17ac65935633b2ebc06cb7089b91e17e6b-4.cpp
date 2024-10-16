void gx_ttfReader__set_font(gx_ttfReader *self, gs_font_type42 *pfont)
{
    self->pfont = pfont;
    self->super.get_metrics = gx_ttfReader__default_get_metrics;
}