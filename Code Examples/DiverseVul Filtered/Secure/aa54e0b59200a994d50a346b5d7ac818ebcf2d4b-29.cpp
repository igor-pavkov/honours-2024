static void update_hb_size(hb_font_t *hb_font, FT_Face face)
{
    hb_font_set_scale (hb_font,
            ((uint64_t) face->size->metrics.x_scale * (uint64_t) face->units_per_EM) >> 16,
            ((uint64_t) face->size->metrics.y_scale * (uint64_t) face->units_per_EM) >> 16);
    hb_font_set_ppem (hb_font, face->size->metrics.x_ppem,
            face->size->metrics.y_ppem);
}