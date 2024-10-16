int update_approximate_cache_glyph_v2_order(const CACHE_GLYPH_V2_ORDER* cache_glyph_v2,
                                            UINT16* flags)
{
	return 8 + cache_glyph_v2->cGlyphs * 32;
}