static int is_new_bm_run(GlyphInfo *info, GlyphInfo *last)
{
    // FIXME: Don't break on glyph substitutions
    return !last || info->effect || info->drawing || last->drawing ||
        strcmp(last->font->desc.family, info->font->desc.family) ||
        last->font->desc.vertical != info->font->desc.vertical ||
        last->face_index != info->face_index ||
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
        last->flags != info->flags;
}