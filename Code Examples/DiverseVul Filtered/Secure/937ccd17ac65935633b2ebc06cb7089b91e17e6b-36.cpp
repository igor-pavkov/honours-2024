static void gx_ttfExport__SetWidth(ttfExport *self, FloatPoint *p)
{
    gx_ttfExport *e = (gx_ttfExport *)self;

    e->w.x = float2fixed(p->x);
    e->w.y = float2fixed(p->y);
}