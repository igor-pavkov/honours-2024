static hb_font_t *get_hb_font(ASS_Shaper *shaper, GlyphInfo *info)
{
    ASS_Font *font = info->font;
    hb_font_t **hb_fonts;

    if (!font->shaper_priv)
        font->shaper_priv = calloc(sizeof(ASS_ShaperFontData), 1);


    hb_fonts = font->shaper_priv->fonts;
    if (!hb_fonts[info->face_index]) {
        hb_fonts[info->face_index] =
            hb_ft_font_create(font->faces[info->face_index], NULL);

        // set up cached metrics access
        font->shaper_priv->metrics_data[info->face_index] =
            calloc(sizeof(struct ass_shaper_metrics_data), 1);
        struct ass_shaper_metrics_data *metrics =
            font->shaper_priv->metrics_data[info->face_index];
        metrics->metrics_cache = shaper->metrics_cache;
        metrics->vertical = info->font->desc.vertical;

        hb_font_funcs_t *funcs = hb_font_funcs_create();
        font->shaper_priv->font_funcs[info->face_index] = funcs;
        hb_font_funcs_set_glyph_func(funcs, get_glyph,
                metrics, NULL);
        hb_font_funcs_set_glyph_h_advance_func(funcs, cached_h_advance,
                metrics, NULL);
        hb_font_funcs_set_glyph_v_advance_func(funcs, cached_v_advance,
                metrics, NULL);
        hb_font_funcs_set_glyph_h_origin_func(funcs, cached_h_origin,
                metrics, NULL);
        hb_font_funcs_set_glyph_v_origin_func(funcs, cached_v_origin,
                metrics, NULL);
        hb_font_funcs_set_glyph_h_kerning_func(funcs, get_h_kerning,
                metrics, NULL);
        hb_font_funcs_set_glyph_v_kerning_func(funcs, get_v_kerning,
                metrics, NULL);
        hb_font_funcs_set_glyph_extents_func(funcs, cached_extents,
                metrics, NULL);
        hb_font_funcs_set_glyph_contour_point_func(funcs, get_contour_point,
                metrics, NULL);
        hb_font_set_funcs(hb_fonts[info->face_index], funcs,
                font->faces[info->face_index], NULL);
    }

    ass_face_set_size(font->faces[info->face_index], info->font_size);
    update_hb_size(hb_fonts[info->face_index], font->faces[info->face_index]);

    // update hash key for cached metrics
    struct ass_shaper_metrics_data *metrics =
        font->shaper_priv->metrics_data[info->face_index];
    metrics->hash_key.font = info->font;
    metrics->hash_key.face_index = info->face_index;
    metrics->hash_key.size = info->font_size;
    metrics->hash_key.scale_x = double_to_d6(info->scale_x);
    metrics->hash_key.scale_y = double_to_d6(info->scale_y);

    return hb_fonts[info->face_index];
}