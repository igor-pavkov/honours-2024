void gx_ttfReader__destroy(gx_ttfReader *self)
{
    gs_free_object(self->memory, self, "gx_ttfReader__destroy");
}