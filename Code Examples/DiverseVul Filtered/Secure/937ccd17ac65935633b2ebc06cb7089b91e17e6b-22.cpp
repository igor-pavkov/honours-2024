static void gx_ttfExport__MoveTo(ttfExport *self, FloatPoint *p)
{
    gx_ttfExport *e = (gx_ttfExport *)self;

    if (!e->error)
        e->error = gx_path_add_point(e->path, float2fixed(p->x), float2fixed(p->y));
}