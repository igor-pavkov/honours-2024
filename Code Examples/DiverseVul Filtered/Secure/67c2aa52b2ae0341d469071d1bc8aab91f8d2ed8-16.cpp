int update_approximate_cache_glyph_order(const CACHE_GLYPH_ORDER* cache_glyph, UINT16* flags)
{
	return 2 + cache_glyph->cGlyphs * 32;
}