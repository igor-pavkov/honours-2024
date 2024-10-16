static void free_render_context(ASS_Renderer *render_priv)
{
    ass_cache_dec_ref(render_priv->state.font);
    free(render_priv->state.family);
    ass_drawing_free(render_priv->state.clip_drawing);

    render_priv->state.font = NULL;
    render_priv->state.family = NULL;
    render_priv->state.clip_drawing = NULL;

    TextInfo *text_info = &render_priv->text_info;
    for (int n = 0; n < text_info->length; n++)
        ass_drawing_free(text_info->glyphs[n].drawing);
    text_info->length = 0;
}