ASS_Shaper *ass_shaper_new(size_t prealloc)
{
    ASS_Shaper *shaper = calloc(sizeof(*shaper), 1);
    if (!shaper)
        return NULL;

    shaper->base_direction = FRIBIDI_PAR_ON;
    if (!check_allocations(shaper, prealloc))
        goto error;

#ifdef CONFIG_HARFBUZZ
    if (!init_features(shaper))
        goto error;
    shaper->metrics_cache = ass_glyph_metrics_cache_create();
    if (!shaper->metrics_cache)
        goto error;
#endif

    return shaper;

error:
    ass_shaper_free(shaper);
    return NULL;
}