static void retrieve_glyphs(ASS_Renderer *render_priv)
{
    GlyphInfo *glyphs = render_priv->text_info.glyphs;
    int i;

    for (i = 0; i < render_priv->text_info.length; i++) {
        GlyphInfo *info = glyphs + i;
        while (info) {
            get_outline_glyph(render_priv, info);
            info = info->next;
        }
        info = glyphs + i;

        // Add additional space after italic to non-italic style changes
        if (i && glyphs[i - 1].italic && !info->italic) {
            int back = i - 1;
            GlyphInfo *og = &glyphs[back];
            while (back && og->bbox.xMax - og->bbox.xMin == 0
                    && og->italic)
                og = &glyphs[--back];
            if (og->bbox.xMax > og->cluster_advance.x)
                og->cluster_advance.x = og->bbox.xMax;
        }

        // add horizontal letter spacing
        info->cluster_advance.x += double_to_d6(info->hspacing *
                render_priv->font_scale * info->orig_scale_x);

        // add displacement for vertical shearing
        info->cluster_advance.y += (info->fay / info->scale_x * info->scale_y) * info->cluster_advance.x;
    }
}