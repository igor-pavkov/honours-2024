BOOL update_write_cache_glyph_order(wStream* s, const CACHE_GLYPH_ORDER* cache_glyph, UINT16* flags)
{
	int i, inf;
	INT16 lsi16;
	const GLYPH_DATA* glyph;
	inf = update_approximate_cache_glyph_order(cache_glyph, flags);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	Stream_Write_UINT8(s, cache_glyph->cacheId); /* cacheId (1 byte) */
	Stream_Write_UINT8(s, cache_glyph->cGlyphs); /* cGlyphs (1 byte) */

	for (i = 0; i < (int)cache_glyph->cGlyphs; i++)
	{
		UINT32 cb;
		glyph = &cache_glyph->glyphData[i];
		Stream_Write_UINT16(s, glyph->cacheIndex); /* cacheIndex (2 bytes) */
		lsi16 = glyph->x;
		Stream_Write_UINT16(s, lsi16); /* x (2 bytes) */
		lsi16 = glyph->y;
		Stream_Write_UINT16(s, lsi16);     /* y (2 bytes) */
		Stream_Write_UINT16(s, glyph->cx); /* cx (2 bytes) */
		Stream_Write_UINT16(s, glyph->cy); /* cy (2 bytes) */
		cb = ((glyph->cx + 7) / 8) * glyph->cy;
		cb += ((cb % 4) > 0) ? 4 - (cb % 4) : 0;
		Stream_Write(s, glyph->aj, cb);
	}

	if (*flags & CG_GLYPH_UNICODE_PRESENT)
	{
		Stream_Zero(s, cache_glyph->cGlyphs * 2);
	}

	return TRUE;
}