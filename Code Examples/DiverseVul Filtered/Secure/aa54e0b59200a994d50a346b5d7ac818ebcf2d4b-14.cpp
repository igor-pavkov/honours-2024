FriBidiStrIndex *ass_shaper_reorder(ASS_Shaper *shaper, TextInfo *text_info)
{
    int i, ret;

    // Initialize reorder map
    for (i = 0; i < text_info->length; i++)
        shaper->cmap[i] = i;

    // Create reorder map line-by-line
    for (i = 0; i < text_info->n_lines; i++) {
        LineInfo *line = text_info->lines + i;
        FriBidiParType dir = FRIBIDI_PAR_ON;

        ret = fribidi_reorder_line(0,
                shaper->ctypes + line->offset, line->len, 0, dir,
                shaper->emblevels + line->offset, NULL,
                shaper->cmap + line->offset);
        if (ret == 0)
            return NULL;
    }

    return shaper->cmap;
}