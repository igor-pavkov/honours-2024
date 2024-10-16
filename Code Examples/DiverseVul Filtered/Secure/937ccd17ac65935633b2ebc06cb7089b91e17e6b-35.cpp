int gx_ttf_outline(ttfFont *ttf, gx_ttfReader *r, gs_font_type42 *pfont, int glyph_index,
        const gs_matrix *m, const gs_log2_scale_point *pscale,
        gx_path *path, bool design_grid)
{
    gx_ttfExport e;
    ttfOutliner o;
    gs_point char_size, subpix_origin;
    gs_matrix post_transform;
    /* Ghostscript proceses a TTC index in gs/lib/gs_ttf.ps, */
    /* so that TTC never comes here. */
    FloatMatrix m1;
    bool dg;
    uint gftt = gs_currentgridfittt(pfont->dir);
    bool ttin = (gftt & 1);
    /*	gs_currentgridfittt values (binary) :
        00 - no grid fitting;
        01 - Grid fit with TT interpreter; On failure warn and render unhinted.
        10 - Interpret in the design grid and then autohint.
        11 - Grid fit with TT interpreter; On failure render autohinted.
    */
    bool auth = (gftt & 2);

    decompose_matrix(pfont, m, pscale, design_grid, &char_size, &subpix_origin, &post_transform, &dg);
    m1.a = post_transform.xx;
    m1.b = post_transform.xy;
    m1.c = post_transform.yx;
    m1.d = post_transform.yy;
    m1.tx = post_transform.tx;
    m1.ty = post_transform.ty;
    e.super.bPoints = false;
    e.super.bOutline = true;
    e.super.MoveTo = gx_ttfExport__MoveTo;
    e.super.LineTo = gx_ttfExport__LineTo;
    e.super.CurveTo = gx_ttfExport__CurveTo;
    e.super.Close = gx_ttfExport__Close;
    e.super.Point = gx_ttfExport__Point;
    e.super.SetWidth = gx_ttfExport__SetWidth;
    e.super.DebugPaint = gx_ttfExport__DebugPaint;
    e.error = 0;
    e.path = path;
    e.w.x = 0;
    e.w.y = 0;
    e.monotonize = auth;
    gx_ttfReader__Reset(r);
    ttfOutliner__init(&o, ttf, &r->super, &e.super, true, false, pfont->WMode != 0);
    switch(ttfOutliner__Outline(&o, glyph_index, subpix_origin.x, subpix_origin.y, &m1)) {
        case fBadInstruction:
            WarnBadInstruction(pfont, glyph_index);
            goto recover;
        case fPatented:
            /* The returned outline did not apply a bytecode (it is not grid-fitted). */
            if (!auth)
                WarnPatented(pfont, ttf, "Some glyphs of the font");
        recover :
            if (!design_grid && auth)
                return grid_fit(pfont->dir->san, path, pfont, pscale, &e, &o);
            /* Falls through. */
        case fNoError:
            if (!design_grid && !ttin && auth)
                return grid_fit(pfont->dir->san, path, pfont, pscale, &e, &o);
            ttfOutliner__DrawGlyphOutline(&o);
            if (e.error)
                return e.error;
            return 0;
        case fMemoryError:
            return_error(gs_error_VMerror);
        case fUnimplemented:
            return_error(gs_error_unregistered);
        default:
            {	int code = r->super.Error(&r->super);

                if (code < 0)
                    return code;
                return_error(gs_error_invalidfont);
            }
    }
}