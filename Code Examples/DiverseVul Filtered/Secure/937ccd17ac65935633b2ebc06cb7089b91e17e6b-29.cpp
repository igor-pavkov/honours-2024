static void gx_ttfExport__LineTo(ttfExport *self, FloatPoint *p)
{
    gx_ttfExport *e = (gx_ttfExport *)self;

    if (!e->error)
        e->error = gx_path_add_line_notes(e->path, float2fixed(p->x), float2fixed(p->y), sn_none);
}