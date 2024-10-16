void ass_shaper_font_data_free(ASS_ShaperFontData *priv)
{
#ifdef CONFIG_HARFBUZZ
    int i;
    for (i = 0; i < ASS_FONT_MAX_FACES; i++)
        if (priv->fonts[i]) {
            free(priv->metrics_data[i]);
            hb_font_destroy(priv->fonts[i]);
            hb_font_funcs_destroy(priv->font_funcs[i]);
        }
    free(priv);
#endif
}