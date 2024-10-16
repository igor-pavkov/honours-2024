static void make_shadow_bitmap(CombinedBitmapInfo *info, ASS_Renderer *render_priv)
{
    if (!(info->filter.flags & FILTER_NONZERO_SHADOW)) {
        if (info->bm && info->bm_o && !(info->filter.flags & FILTER_BORDER_STYLE_3)) {
            fix_outline(info->bm, info->bm_o);
        } else if (info->bm_o && !(info->filter.flags & FILTER_NONZERO_BORDER)) {
            ass_free_bitmap(info->bm_o);
            info->bm_o = 0;
        }
        return;
    }

    // Create shadow and fix outline as needed
    if (info->bm && info->bm_o && !(info->filter.flags & FILTER_BORDER_STYLE_3)) {
        info->bm_s = copy_bitmap(render_priv->engine, info->bm_o);
        fix_outline(info->bm, info->bm_o);
    } else if (info->bm_o && (info->filter.flags & FILTER_NONZERO_BORDER)) {
        info->bm_s = copy_bitmap(render_priv->engine, info->bm_o);
    } else if (info->bm_o) {
        info->bm_s = info->bm_o;
        info->bm_o = 0;
    } else if (info->bm)
        info->bm_s = copy_bitmap(render_priv->engine, info->bm);

    if (!info->bm_s)
        return;

    // Works right even for negative offsets
    // '>>' rounds toward negative infinity, '&' returns correct remainder
    info->bm_s->left += info->filter.shadow.x >> 6;
    info->bm_s->top  += info->filter.shadow.y >> 6;
    shift_bitmap(info->bm_s, info->filter.shadow.x & SUBPIXEL_MASK, info->filter.shadow.y & SUBPIXEL_MASK);
}