static int parse_events(ASS_Renderer *render_priv, ASS_Event *event)
{
    TextInfo *text_info = &render_priv->text_info;
    ASS_Drawing *drawing = NULL;
    unsigned code;
    char *p, *q;
    int i;

    p = event->Text;

    // Event parsing.
    while (1) {
        // get next char, executing style override
        // this affects render_context
        code = 0;
        while (*p) {
            if ((*p == '{') && (q = strchr(p, '}'))) {
                while (p < q)
                    p = parse_tag(render_priv, p, q, 1.);
                assert(*p == '}');
                p++;
            } else if (render_priv->state.drawing_scale) {
                q = p;
                if (*p == '{')
                    q++;
                while ((*q != '{') && (*q != 0))
                    q++;
                if (!drawing) {
                    drawing = ass_drawing_new(render_priv->library,
                                              render_priv->ftlibrary);
                    if (!drawing)
                        return 1;
                }
                ass_drawing_set_text(drawing, p, q - p);
                code = 0xfffc; // object replacement character
                p = q;
                break;
            } else {
                code = get_next_char(render_priv, &p);
                break;
            }
        }

        if (code == 0)
            break;

        // face could have been changed in get_next_char
        if (!render_priv->state.font) {
            free_render_context(render_priv);
            ass_drawing_free(drawing);
            return 1;
        }

        if (text_info->length >= text_info->max_glyphs) {
            // Raise maximum number of glyphs
            text_info->max_glyphs *= 2;
            text_info->glyphs =
                realloc(text_info->glyphs,
                        sizeof(GlyphInfo) * text_info->max_glyphs);
        }

        GlyphInfo *info = &text_info->glyphs[text_info->length];

        // Clear current GlyphInfo
        memset(info, 0, sizeof(GlyphInfo));

        // Parse drawing
        if (drawing && drawing->text) {
            drawing->scale_x = render_priv->state.scale_x *
                                     render_priv->font_scale;
            drawing->scale_y = render_priv->state.scale_y *
                                     render_priv->font_scale;
            drawing->scale = render_priv->state.drawing_scale;
            drawing->pbo = render_priv->state.pbo;
            info->drawing = drawing;
            drawing = NULL;
        }

        // Fill glyph information
        info->symbol = code;
        info->font = render_priv->state.font;
        if (!info->drawing)
            ass_cache_inc_ref(info->font);
        for (i = 0; i < 4; ++i) {
            uint32_t clr = render_priv->state.c[i];
            // VSFilter compatibility: apply fade only when it's positive
            if (render_priv->state.fade > 0)
                change_alpha(&clr,
                             mult_alpha(_a(clr), render_priv->state.fade), 1.);
            info->c[i] = clr;
        }

        info->effect_type = render_priv->state.effect_type;
        info->effect_timing = render_priv->state.effect_timing;
        info->effect_skip_timing = render_priv->state.effect_skip_timing;
        info->font_size =
            render_priv->state.font_size * render_priv->font_scale;
        info->be = render_priv->state.be;
        info->blur = render_priv->state.blur;
        info->shadow_x = render_priv->state.shadow_x;
        info->shadow_y = render_priv->state.shadow_y;
        info->scale_x = info->orig_scale_x = render_priv->state.scale_x;
        info->scale_y = info->orig_scale_y = render_priv->state.scale_y;
        info->border_style = render_priv->state.border_style;
        info->border_x= render_priv->state.border_x;
        info->border_y = render_priv->state.border_y;
        info->hspacing = render_priv->state.hspacing;
        info->bold = render_priv->state.bold;
        info->italic = render_priv->state.italic;
        info->flags = render_priv->state.flags;
        info->frx = render_priv->state.frx;
        info->fry = render_priv->state.fry;
        info->frz = render_priv->state.frz;
        info->fax = render_priv->state.fax;
        info->fay = render_priv->state.fay;

        if (!info->drawing)
            fix_glyph_scaling(render_priv, info);

        text_info->length++;

        render_priv->state.effect_type = EF_NONE;
        render_priv->state.effect_timing = 0;
        render_priv->state.effect_skip_timing = 0;
    }

    ass_drawing_free(drawing);

    return 0;
}