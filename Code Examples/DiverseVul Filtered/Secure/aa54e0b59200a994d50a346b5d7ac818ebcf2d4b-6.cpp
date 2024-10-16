static void shape_fribidi(ASS_Shaper *shaper, GlyphInfo *glyphs, size_t len)
{
    int i;
    FriBidiJoiningType *joins = calloc(sizeof(*joins), len);

    // shape on codepoint level
    fribidi_get_joining_types(shaper->event_text, len, joins);
    fribidi_join_arabic(shaper->ctypes, len, shaper->emblevels, joins);
    fribidi_shape(FRIBIDI_FLAGS_DEFAULT | FRIBIDI_FLAGS_ARABIC,
            shaper->emblevels, len, joins, shaper->event_text);

    // update indexes
    for (i = 0; i < len; i++) {
        GlyphInfo *info = glyphs + i;
        FT_Face face = info->font->faces[info->face_index];
        info->symbol = shaper->event_text[i];
        info->glyph_index = FT_Get_Char_Index(face, ass_font_index_magic(face, shaper->event_text[i]));
    }

    free(joins);
}