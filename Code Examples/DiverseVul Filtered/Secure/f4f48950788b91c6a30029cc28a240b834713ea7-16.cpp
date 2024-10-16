static void measure_text(ASS_Renderer *render_priv)
{
    TextInfo *text_info = &render_priv->text_info;
    int cur_line = 0;
    double max_asc = 0., max_desc = 0.;
    GlyphInfo *last = NULL;
    int i;
    int empty_line = 1;
    text_info->height = 0.;
    for (i = 0; i < text_info->length + 1; ++i) {
        if ((i == text_info->length) || text_info->glyphs[i].linebreak) {
            if (empty_line && cur_line > 0 && last) {
                max_asc = d6_to_double(last->asc) / 2.0;
                max_desc = d6_to_double(last->desc) / 2.0;
            }
            text_info->lines[cur_line].asc = max_asc;
            text_info->lines[cur_line].desc = max_desc;
            text_info->height += max_asc + max_desc;
            cur_line++;
            max_asc = max_desc = 0.;
            empty_line = 1;
        }
        if (i < text_info->length) {
            GlyphInfo *cur = text_info->glyphs + i;
            if (d6_to_double(cur->asc) > max_asc)
                max_asc = d6_to_double(cur->asc);
            if (d6_to_double(cur->desc) > max_desc)
                max_desc = d6_to_double(cur->desc);
            if (cur->symbol != '\n' && cur->symbol != 0) {
                empty_line = 0;
                last = cur;
            }
        }
    }
    text_info->height +=
        (text_info->n_lines -
         1) * render_priv->settings.line_spacing;
}