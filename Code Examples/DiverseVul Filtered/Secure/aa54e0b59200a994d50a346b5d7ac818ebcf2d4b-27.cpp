static void ass_shaper_skip_characters(TextInfo *text_info)
{
    int i;
    GlyphInfo *glyphs = text_info->glyphs;

    for (i = 0; i < text_info->length; i++) {
        // Skip direction override control characters
        if ((glyphs[i].symbol <= 0x202e && glyphs[i].symbol >= 0x202a)
                || (glyphs[i].symbol <= 0x200f && glyphs[i].symbol >= 0x200b)
                || (glyphs[i].symbol <= 0x2063 && glyphs[i].symbol >= 0x2060)
                || glyphs[i].symbol == 0xfeff
                || glyphs[i].symbol == 0x00ad
                || glyphs[i].symbol == 0x034f) {
            glyphs[i].symbol = 0;
            glyphs[i].skip++;
        }
    }
}