static void check_cache_limits(ASS_Renderer *priv, CacheStore *cache)
{
    ass_cache_cut(cache->composite_cache, cache->composite_max_size);
    ass_cache_cut(cache->bitmap_cache, cache->bitmap_max_size);
    ass_cache_cut(cache->outline_cache, cache->glyph_max);
}