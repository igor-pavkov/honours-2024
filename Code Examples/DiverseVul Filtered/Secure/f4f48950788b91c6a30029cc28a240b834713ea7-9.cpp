static ASS_Image *render_text(ASS_Renderer *render_priv)
{
    ASS_Image *head;
    ASS_Image **tail = &head;
    unsigned n_bitmaps = render_priv->text_info.n_bitmaps;
    CombinedBitmapInfo *bitmaps = render_priv->text_info.combined_bitmaps;

    for (unsigned i = 0; i < n_bitmaps; i++) {
        CombinedBitmapInfo *info = &bitmaps[i];
        if (!info->bm_s || render_priv->state.border_style == 4)
            continue;

        tail =
            render_glyph(render_priv, info->bm_s, info->x, info->y, info->c[3], 0,
                         1000000, tail, IMAGE_TYPE_SHADOW, info->image);
    }

    for (unsigned i = 0; i < n_bitmaps; i++) {
        CombinedBitmapInfo *info = &bitmaps[i];
        if (!info->bm_o)
            continue;

        if ((info->effect_type == EF_KARAOKE_KO)
                && (info->effect_timing <= info->first_pos_x)) {
            // do nothing
        } else {
            tail =
                render_glyph(render_priv, info->bm_o, info->x, info->y, info->c[2],
                             0, 1000000, tail, IMAGE_TYPE_OUTLINE, info->image);
        }
    }

    for (unsigned i = 0; i < n_bitmaps; i++) {
        CombinedBitmapInfo *info = &bitmaps[i];
        if (!info->bm)
            continue;

        if ((info->effect_type == EF_KARAOKE)
                || (info->effect_type == EF_KARAOKE_KO)) {
            if (info->effect_timing > info->first_pos_x)
                tail =
                    render_glyph(render_priv, info->bm, info->x, info->y,
                                 info->c[0], 0, 1000000, tail,
                                 IMAGE_TYPE_CHARACTER, info->image);
            else
                tail =
                    render_glyph(render_priv, info->bm, info->x, info->y,
                                 info->c[1], 0, 1000000, tail,
                                 IMAGE_TYPE_CHARACTER, info->image);
        } else if (info->effect_type == EF_KARAOKE_KF) {
            tail =
                render_glyph(render_priv, info->bm, info->x, info->y, info->c[0],
                             info->c[1], info->effect_timing, tail,
                             IMAGE_TYPE_CHARACTER, info->image);
        } else
            tail =
                render_glyph(render_priv, info->bm, info->x, info->y, info->c[0],
                             0, 1000000, tail, IMAGE_TYPE_CHARACTER, info->image);
    }

    for (unsigned i = 0; i < n_bitmaps; i++)
        ass_cache_dec_ref(bitmaps[i].image);

    *tail = 0;
    blend_vector_clip(render_priv, head);

    return head;
}