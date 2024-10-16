void ass_shaper_info(ASS_Library *lib)
{
    ass_msg(lib, MSGL_INFO, "Shaper: FriBidi "
            FRIBIDI_VERSION " (SIMPLE)"
#ifdef CONFIG_HARFBUZZ
            " HarfBuzz-ng %s (COMPLEX)", hb_version_string()
#endif
           );
}