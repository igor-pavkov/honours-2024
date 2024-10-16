static void set_run_features(ASS_Shaper *shaper, GlyphInfo *info)
{
    // enable vertical substitutions for @font runs
    if (info->font->desc.vertical)
        shaper->features[VERT].value = shaper->features[VKNA].value = 1;
    else
        shaper->features[VERT].value = shaper->features[VKNA].value = 0;

    // disable ligatures if horizontal spacing is non-standard
    if (info->hspacing)
        shaper->features[LIGA].value = shaper->features[CLIG].value = 0;
    else
        shaper->features[LIGA].value = shaper->features[CLIG].value = 1;
}