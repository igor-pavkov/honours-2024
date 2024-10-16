void ass_renderer_done(ASS_Renderer *render_priv)
{
    ass_frame_unref(render_priv->images_root);
    ass_frame_unref(render_priv->prev_images_root);

    ass_cache_done(render_priv->cache.composite_cache);
    ass_cache_done(render_priv->cache.bitmap_cache);
    ass_cache_done(render_priv->cache.outline_cache);
    ass_shaper_free(render_priv->shaper);
    ass_cache_done(render_priv->cache.font_cache);

#if CONFIG_RASTERIZER
    rasterizer_done(&render_priv->rasterizer);
#endif

    if (render_priv->state.stroker) {
        FT_Stroker_Done(render_priv->state.stroker);
        render_priv->state.stroker = 0;
    }
    if (render_priv->fontselect)
        ass_fontselect_free(render_priv->fontselect);
    if (render_priv->ftlibrary)
        FT_Done_FreeType(render_priv->ftlibrary);
    free(render_priv->eimg);
    free(render_priv->text_info.glyphs);
    free(render_priv->text_info.lines);

    free(render_priv->text_info.combined_bitmaps);

    free(render_priv->settings.default_font);
    free(render_priv->settings.default_family);

    free(render_priv->user_override_style.FontName);

    free(render_priv);
}