void ass_shaper_free(ASS_Shaper *shaper)
{
#ifdef CONFIG_HARFBUZZ
    ass_cache_done(shaper->metrics_cache);
    free(shaper->features);
#endif
    free(shaper->event_text);
    free(shaper->ctypes);
    free(shaper->emblevels);
    free(shaper->cmap);
    free(shaper);
}