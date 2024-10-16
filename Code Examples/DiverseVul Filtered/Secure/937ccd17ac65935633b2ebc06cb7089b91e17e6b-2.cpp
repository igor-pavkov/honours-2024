static void WarnBadInstruction(gs_font_type42 *pfont, int glyph_index)
{
    char buf[gs_font_name_max + 1];
    int l;
    gs_font_type42 *base_font = pfont;

    while ((gs_font_type42 *)base_font->base != base_font)
        base_font = (gs_font_type42 *)base_font->base;
    if (!base_font->data.warning_bad_instruction) {
        l = min(sizeof(buf) - 1, base_font->font_name.size);
        memcpy(buf, base_font->font_name.chars, l);
        buf[l] = 0;
        if (glyph_index >= 0)
            emprintf2(pfont->memory,
                      "Failed to interpret TT instructions for glyph index %d of font %s. "
                      "Continue ignoring instructions of the font.\n",
                      glyph_index, buf);
        else
            emprintf1(pfont->memory,
                      "Failed to interpret TT instructions in font %s. "
                      "Continue ignoring instructions of the font.\n",
                      buf);
        base_font->data.warning_bad_instruction = true;
    }
}