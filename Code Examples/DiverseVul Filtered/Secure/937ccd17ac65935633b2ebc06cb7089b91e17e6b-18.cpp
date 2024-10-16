static int grid_fit(gx_device_spot_analyzer *padev, gx_path *path,
        gs_font_type42 *pfont, const gs_log2_scale_point *pscale, gx_ttfExport *e, ttfOutliner *o)
{
    /* Not completed yet. */
    gs_gstate gs_stub;
    gx_fill_params params;
    gx_device_color devc_stub;
    int code;
    t1_hinter_aux h;
    gs_matrix m, fm, fmb;
    gs_matrix_fixed ctm_temp;
    bool atp = gs_currentaligntopixels(pfont->dir);
    int FontType = 1; /* Will apply Type 1 hinter. */
    fixed sbx = 0, sby = 0; /* stub */
    double scale = 1.0 / o->pFont->nUnitsPerEm;
    gs_fixed_rect bbox;

    m.xx = o->post_transform.a;
    m.xy = o->post_transform.b;
    m.yx = o->post_transform.c;
    m.yy = o->post_transform.d;
    m.tx = o->post_transform.tx;
    m.ty = o->post_transform.ty;
    code = gs_matrix_fixed_from_matrix(&ctm_temp, &m);
    if (code < 0)
        return code;
    code = gs_matrix_scale(&pfont->FontMatrix, scale, scale, &fm);
    if (code < 0)
        return code;
    code = gs_matrix_scale(&pfont->base->FontMatrix, scale, scale, &fmb);
    if (code < 0)
        return code;
    t1_hinter__init(&h.super, path); /* Will export to */
    code = t1_hinter__set_mapping(&h.super, &ctm_temp,
                        &fm, &fmb,
                        pscale->x, pscale->x, 0, 0,
                        ctm_temp.tx_fixed, ctm_temp.ty_fixed, atp);
    if (code < 0)
        return code;
    if (!h.super.disable_hinting) {
        o->post_transform.a = o->post_transform.d = 1;
        o->post_transform.b = o->post_transform.c = 0;
        o->post_transform.tx = o->post_transform.ty = 0;
        ttfOutliner__DrawGlyphOutline(o);
        if (e->error)
            return e->error;
        code = t1_hinter__set_font42_data(&h.super, FontType, &pfont->data, false);
        if (code < 0)
            return code;
        code = t1_hinter__sbw(&h.super, sbx, sby, e->w.x, e->w.y);
        if (code < 0)
            return code;
        code = gx_path_bbox(path, &bbox);
        if (code < 0)
            return code;
        memset(&gs_stub, 0, sizeof(gs_stub));
        gs_stub.memory = padev->memory;
        set_nonclient_dev_color(&devc_stub, 1);
        params.rule = gx_rule_winding_number;
        params.adjust.x = params.adjust.y = 0;
        params.flatness = fixed2float(max(bbox.q.x - bbox.p.x, bbox.q.y - bbox.p.y)) / 100.0;

        for (h.transpose = 0; h.transpose < 2; h.transpose++) {
            h.midx = (padev->xmin + padev->xmax) / 2;
            if (h.transpose)
                transpose_path(path);
            gx_san_begin(padev);
            code = dev_proc(padev, fill_path)((gx_device *)padev,
                            &gs_stub, path, &params, &devc_stub, NULL);
            gx_san_end(padev);
            if (code >= 0)
                code = gx_san_generate_stems(padev, OVERALL_HINT && h.transpose,
                                &h, stem_hint_handler);
            if (h.transpose)
                transpose_path(path);
            if (code < 0)
                return code;
        }

        /*  fixme : Storing hints permanently would be useful.
            Note that if (gftt & 1), the outline and hints are already scaled.
        */
        code = path_to_hinter(&h.super, path);
        if (code < 0)
            return code;
        code = gx_path_new(path);
        if (code < 0)
            return code;
        code = t1_hinter__endglyph(&h.super);
    } else {
        ttfOutliner__DrawGlyphOutline(o);
        if (e->error)
            return e->error;
    }
    return code;
}