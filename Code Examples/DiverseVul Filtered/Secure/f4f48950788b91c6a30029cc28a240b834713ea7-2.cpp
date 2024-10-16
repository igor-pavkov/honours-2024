static void preliminary_layout(ASS_Renderer *render_priv)
{
    FT_Vector pen;
    int i;

    pen.x = 0;
    pen.y = 0;
    for (i = 0; i < render_priv->text_info.length; i++) {
        GlyphInfo *info = render_priv->text_info.glyphs + i;
        FT_Vector cluster_pen = pen;
        while (info) {
            info->pos.x = cluster_pen.x;
            info->pos.y = cluster_pen.y;

            cluster_pen.x += info->advance.x;
            cluster_pen.y += info->advance.y;

            // fill bitmap hash
            info->hash_key.type = BITMAP_OUTLINE;
            fill_bitmap_hash(render_priv, info, &info->hash_key.u.outline);

            info = info->next;
        }
        info = render_priv->text_info.glyphs + i;
        pen.x += info->cluster_advance.x;
        pen.y += info->cluster_advance.y;
    }
}