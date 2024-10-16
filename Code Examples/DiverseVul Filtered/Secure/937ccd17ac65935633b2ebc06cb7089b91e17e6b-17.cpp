static void gx_ttfExport__CurveTo(ttfExport *self, FloatPoint *p0, FloatPoint *p1, FloatPoint *p2)
{
    gx_ttfExport *e = (gx_ttfExport *)self;

    if (!e->error) {
        if (e->monotonize) {
            curve_segment s;

            s.notes = sn_none;
            s.p1.x = float2fixed(p0->x), s.p1.y = float2fixed(p0->y),
            s.p2.x = float2fixed(p1->x), s.p2.y = float2fixed(p1->y),
            s.pt.x = float2fixed(p2->x), s.pt.y = float2fixed(p2->y);
            e->error = gx_curve_monotonize(e->path, &s);
        } else
            e->error = gx_path_add_curve_notes(e->path, float2fixed(p0->x), float2fixed(p0->y),
                                     float2fixed(p1->x), float2fixed(p1->y),
                                     float2fixed(p2->x), float2fixed(p2->y), sn_none);
    }
}