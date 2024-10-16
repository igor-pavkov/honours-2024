static CACHE_GLYPH_ORDER* update_read_cache_glyph_order(rdpUpdate* update, wStream* s, UINT16 flags)
{
	UINT32 i;
	CACHE_GLYPH_ORDER* cache_glyph_order = calloc(1, sizeof(CACHE_GLYPH_ORDER));

	if (!cache_glyph_order || !update || !s)
		goto fail;

	if (Stream_GetRemainingLength(s) < 2)
		goto fail;

	Stream_Read_UINT8(s, cache_glyph_order->cacheId); /* cacheId (1 byte) */
	Stream_Read_UINT8(s, cache_glyph_order->cGlyphs); /* cGlyphs (1 byte) */

	for (i = 0; i < cache_glyph_order->cGlyphs; i++)
	{
		GLYPH_DATA* glyph = &cache_glyph_order->glyphData[i];

		if (Stream_GetRemainingLength(s) < 10)
			goto fail;

		Stream_Read_UINT16(s, glyph->cacheIndex);
		Stream_Read_INT16(s, glyph->x);
		Stream_Read_INT16(s, glyph->y);
		Stream_Read_UINT16(s, glyph->cx);
		Stream_Read_UINT16(s, glyph->cy);
		glyph->cb = ((glyph->cx + 7) / 8) * glyph->cy;
		glyph->cb += ((glyph->cb % 4) > 0) ? 4 - (glyph->cb % 4) : 0;

		if (Stream_GetRemainingLength(s) < glyph->cb)
			goto fail;

		glyph->aj = (BYTE*)malloc(glyph->cb);

		if (!glyph->aj)
			goto fail;

		Stream_Read(s, glyph->aj, glyph->cb);
	}

	if ((flags & CG_GLYPH_UNICODE_PRESENT) && (cache_glyph_order->cGlyphs > 0))
	{
		cache_glyph_order->unicodeCharacters = calloc(cache_glyph_order->cGlyphs, sizeof(WCHAR));

		if (!cache_glyph_order->unicodeCharacters)
			goto fail;

		if (Stream_GetRemainingLength(s) < sizeof(WCHAR) * cache_glyph_order->cGlyphs)
			goto fail;

		Stream_Read_UTF16_String(s, cache_glyph_order->unicodeCharacters,
		                         cache_glyph_order->cGlyphs);
	}

	return cache_glyph_order;
fail:
	free_cache_glyph_order(update->context, cache_glyph_order);
	return NULL;
}