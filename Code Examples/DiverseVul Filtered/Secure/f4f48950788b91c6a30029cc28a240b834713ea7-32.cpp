static void align_lines(ASS_Renderer *render_priv, double max_text_width)
{
    TextInfo *text_info = &render_priv->text_info;
    GlyphInfo *glyphs = text_info->glyphs;
    int i, j;
    double width = 0;
    int last_break = -1;
    int halign = render_priv->state.alignment & 3;

    if (render_priv->state.evt_type == EVENT_HSCROLL)
        return;

    for (i = 0; i <= text_info->length; ++i) {   // (text_info->length + 1) is the end of the last line
        if ((i == text_info->length) || glyphs[i].linebreak) {
            double shift = 0;
            if (halign == HALIGN_LEFT) {    // left aligned, no action
                shift = 0;
            } else if (halign == HALIGN_RIGHT) {    // right aligned
                shift = max_text_width - width;
            } else if (halign == HALIGN_CENTER) {   // centered
                shift = (max_text_width - width) / 2.0;
            }
            for (j = last_break + 1; j < i; ++j) {
                GlyphInfo *info = glyphs + j;
                while (info) {
                    info->pos.x += double_to_d6(shift);
                    info = info->next;
                }
            }
            last_break = i - 1;
            width = 0;
        }
        if (i < text_info->length && !glyphs[i].skip &&
                glyphs[i].symbol != '\n' && glyphs[i].symbol != 0) {
            width += d6_to_double(glyphs[i].cluster_advance.x);
        }
    }
}