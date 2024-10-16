void ass_shaper_find_runs(ASS_Shaper *shaper, ASS_Renderer *render_priv,
                          GlyphInfo *glyphs, size_t len)
{
    int i;
    int shape_run = 0;

#ifdef CONFIG_HARFBUZZ
    ass_shaper_determine_script(shaper, glyphs, len);
#endif

    // find appropriate fonts for the shape runs
    for (i = 0; i < len; i++) {
        GlyphInfo *last = glyphs + i - 1;
        GlyphInfo *info = glyphs + i;
        // skip drawings
        if (info->symbol == 0xfffc)
            continue;
        // set size and get glyph index
        ass_font_get_index(render_priv->fontselect, info->font,
                info->symbol, &info->face_index, &info->glyph_index);
        // shape runs break on: xbord, ybord, xshad, yshad,
        // all four colors, all four alphas, be, blur, fn, fs,
        // fscx, fscy, fsp, bold, italic, underline, strikeout,
        // frx, fry, frz, fax, fay, karaoke start, karaoke type,
        // and on every line break
        if (i > 0 && (last->font != info->font ||
                    last->face_index != info->face_index ||
                    last->script != info->script ||
                    last->font_size != info->font_size ||
                    last->c[0] != info->c[0] ||
                    last->c[1] != info->c[1] ||
                    last->c[2] != info->c[2] ||
                    last->c[3] != info->c[3] ||
                    last->be != info->be ||
                    last->blur != info->blur ||
                    last->shadow_x != info->shadow_x ||
                    last->shadow_y != info->shadow_y ||
                    last->frx != info->frx ||
                    last->fry != info->fry ||
                    last->frz != info->frz ||
                    last->fax != info->fax ||
                    last->fay != info->fay ||
                    last->scale_x != info->scale_x ||
                    last->scale_y != info->scale_y ||
                    last->border_style != info->border_style ||
                    last->border_x != info->border_x ||
                    last->border_y != info->border_y ||
                    last->hspacing != info->hspacing ||
                    last->italic != info->italic ||
                    last->bold != info->bold ||
                    last->flags != info->flags))
            shape_run++;
        info->shape_run_id = shape_run;
    }
}