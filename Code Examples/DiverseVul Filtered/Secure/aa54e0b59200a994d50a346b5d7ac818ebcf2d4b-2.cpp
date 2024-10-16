int ass_shaper_shape(ASS_Shaper *shaper, TextInfo *text_info)
{
    int i, ret, last_break;
    FriBidiParType dir;
    GlyphInfo *glyphs = text_info->glyphs;

    if (!check_allocations(shaper, text_info->length))
        return -1;

    // Get bidi character types and embedding levels
    last_break = 0;
    for (i = 0; i < text_info->length; i++) {
        shaper->event_text[i] = glyphs[i].symbol;
        // embedding levels should be calculated paragraph by paragraph
        if (glyphs[i].symbol == '\n' || i == text_info->length - 1) {
            dir = shaper->base_direction;
            fribidi_get_bidi_types(shaper->event_text + last_break,
                    i - last_break + 1, shaper->ctypes + last_break);
            ret = fribidi_get_par_embedding_levels(shaper->ctypes + last_break,
                    i - last_break + 1, &dir, shaper->emblevels + last_break);
            if (ret == 0)
                return -1;
            last_break = i + 1;
        }
    }

    // add embedding levels to shape runs for final runs
    for (i = 0; i < text_info->length; i++) {
        glyphs[i].shape_run_id += shaper->emblevels[i];
    }

#ifdef CONFIG_HARFBUZZ
    switch (shaper->shaping_level) {
    case ASS_SHAPING_SIMPLE:
        shape_fribidi(shaper, glyphs, text_info->length);
        ass_shaper_skip_characters(text_info);
        break;
    case ASS_SHAPING_COMPLEX:
        shape_harfbuzz(shaper, glyphs, text_info->length);
        break;
    }
#else
        shape_fribidi(shaper, glyphs, text_info->length);
        ass_shaper_skip_characters(text_info);
#endif

    return 0;
}