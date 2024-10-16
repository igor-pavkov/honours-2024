static void reorder_text(ASS_Renderer *render_priv)
{
    TextInfo *text_info = &render_priv->text_info;
    FT_Vector pen;
    int i;

    FriBidiStrIndex *cmap = ass_shaper_reorder(render_priv->shaper, text_info);
    if (!cmap) {
        ass_msg(render_priv->library, MSGL_ERR, "Failed to reorder text");
        ass_shaper_cleanup(render_priv->shaper, text_info);
        free_render_context(render_priv);
        return;
    }

    // Reposition according to the map
    pen.x = 0;
    pen.y = 0;
    int lineno = 1;
    double last_pen_x = 0;
    double last_fay = 0;
    for (i = 0; i < text_info->length; i++) {
        GlyphInfo *info = text_info->glyphs + cmap[i];
        if (text_info->glyphs[i].linebreak) {
            pen.y -= (last_fay / info->scale_x * info->scale_y) * (pen.x - last_pen_x);
            last_pen_x = pen.x = 0;
            pen.y += double_to_d6(text_info->lines[lineno-1].desc);
            pen.y += double_to_d6(text_info->lines[lineno].asc);
            pen.y += double_to_d6(render_priv->settings.line_spacing);
            lineno++;
        }
        else if (last_fay != info->fay) {
            pen.y -= (last_fay / info->scale_x * info->scale_y) * (pen.x - last_pen_x);
            last_pen_x = pen.x;
        }
        last_fay = info->fay;
        if (info->skip) continue;
        FT_Vector cluster_pen = pen;
        while (info) {
            info->pos.x = info->offset.x + cluster_pen.x;
            info->pos.y = info->offset.y + cluster_pen.y;
            cluster_pen.x += info->advance.x;
            cluster_pen.y += info->advance.y;
            info = info->next;
        }
        info = text_info->glyphs + cmap[i];
        pen.x += info->cluster_advance.x;
        pen.y += info->cluster_advance.y;
    }
}