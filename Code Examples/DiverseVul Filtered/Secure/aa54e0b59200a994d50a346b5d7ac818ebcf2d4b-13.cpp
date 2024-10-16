void ass_shaper_set_language(ASS_Shaper *shaper, const char *code)
{
#ifdef CONFIG_HARFBUZZ
    hb_language_t lang;

    if (code)
        lang = hb_language_from_string(code, -1);
    else
        lang = HB_LANGUAGE_INVALID;

    shaper->language = lang;
#endif
}