void ass_shaper_determine_script(ASS_Shaper *shaper, GlyphInfo *glyphs,
                                  size_t len)
{
    int i;
    int backwards_scan = 0;
    hb_unicode_funcs_t *ufuncs = hb_unicode_funcs_get_default();
    hb_script_t last_script = HB_SCRIPT_UNKNOWN;

    // determine script (forward scan)
    for (i = 0; i < len; i++) {
        GlyphInfo *info = glyphs + i;
        info->script = hb_unicode_script(ufuncs, info->symbol);

        // common/inherit codepoints inherit script from context
        if (info->script == HB_SCRIPT_COMMON ||
                info->script == HB_SCRIPT_INHERITED) {
            // unknown is not a valid context
            if (last_script != HB_SCRIPT_UNKNOWN)
                info->script = last_script;
            else
                // do a backwards scan to check if next codepoint
                // contains a valid script for context
                backwards_scan = 1;
        } else {
            last_script = info->script;
        }
    }

    // determine script (backwards scan, if needed)
    last_script = HB_SCRIPT_UNKNOWN;
    for (i = len - 1; i >= 0 && backwards_scan; i--) {
        GlyphInfo *info = glyphs + i;

        // common/inherit codepoints inherit script from context
        if (info->script == HB_SCRIPT_COMMON ||
                info->script == HB_SCRIPT_INHERITED) {
            // unknown script is not a valid context
            if (last_script != HB_SCRIPT_UNKNOWN)
                info->script = last_script;
        } else {
            last_script = info->script;
        }
    }
}