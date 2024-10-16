static void decompose_matrix(const gs_font_type42 *pfont, const gs_matrix * char_tm,
    const gs_log2_scale_point *log2_scale, bool design_grid,
    gs_point *char_size, gs_point *subpix_origin, gs_matrix *post_transform, bool *dg)
{
    /*
     *	char_tm maps to subpixels.
     */
    /*
     *	We use a Free Type 1 True Type interpreter, which cannot perform
     *	a grid-fitting with skewing/rotation. It appears acceptable
     *	because we want to minimize invocations of patented instructions.
     *	We believe that skewing/rotation requires the patented intrivial cases
     *	of projection/freedom vectors.
     */
    int scale_x = 1 << log2_scale->x;
    int scale_y = 1 << log2_scale->y;
    bool atp = gs_currentaligntopixels(pfont->dir);
    bool design_grid1;

    char_size->x = hypot(char_tm->xx, char_tm->xy);
    char_size->y = hypot(char_tm->yx, char_tm->yy);
    if (char_size->x <= 2 && char_size->y <= 2) {
        /* Disable the grid fitting for very small fonts. */
        design_grid1 = true;
    } else
        design_grid1 = design_grid || !(gs_currentgridfittt(pfont->dir) & 1);
    *dg = design_grid1;
    subpix_origin->x = (atp ? 0 : reminder(char_tm->tx, scale_x) / scale_x);
    subpix_origin->y = (atp ? 0 : reminder(char_tm->ty, scale_y) / scale_y);
    post_transform->xx = char_tm->xx / (design_grid1 ? 1 : char_size->x);
    post_transform->xy = char_tm->xy / (design_grid1 ? 1 : char_size->x);
    post_transform->yx = char_tm->yx / (design_grid1 ? 1 : char_size->y);
    post_transform->yy = char_tm->yy / (design_grid1 ? 1 : char_size->y);
    post_transform->tx = char_tm->tx - subpix_origin->x;
    post_transform->ty = char_tm->ty - subpix_origin->y;
}