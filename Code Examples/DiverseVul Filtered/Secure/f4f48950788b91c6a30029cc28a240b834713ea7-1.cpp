static void draw_opaque_box(ASS_Renderer *render_priv, GlyphInfo *info,
                            int asc, int desc, ASS_Outline *ol,
                            FT_Vector advance, int sx, int sy)
{
    int adv = advance.x;
    double scale_y = info->orig_scale_y;
    double scale_x = info->orig_scale_x;

    // to avoid gaps
    sx = FFMAX(64, sx);
    sy = FFMAX(64, sy);

    // Emulate the WTFish behavior of VSFilter, i.e. double-scale
    // the sizes of the opaque box.
    adv += double_to_d6(info->hspacing * render_priv->font_scale * scale_x);
    adv *= scale_x;
    sx *= scale_x;
    sy *= scale_y;
    desc *= scale_y;
    desc += asc * (scale_y - 1.0);

    FT_Vector points[4] = {
        { .x = -sx,         .y = asc + sy },
        { .x = adv + sx,    .y = asc + sy },
        { .x = adv + sx,    .y = -desc - sy },
        { .x = -sx,         .y = -desc - sy },
    };

    ol->n_points = ol->n_contours = 0;
    if (!outline_alloc(ol, 4, 1))
        return;
    for (int i = 0; i < 4; ++i) {
        ol->points[ol->n_points] = points[i];
        ol->tags[ol->n_points++] = 1;
    }
    ol->contours[ol->n_contours++] = ol->n_points - 1;
}