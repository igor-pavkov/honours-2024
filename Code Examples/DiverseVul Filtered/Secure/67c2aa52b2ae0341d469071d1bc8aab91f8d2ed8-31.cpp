static CACHE_GLYPH_V2_ORDER* update_read_cache_glyph_v2_order(rdpUpdate* update, wStream* s,
                                                              UINT16 flags)
{
	UINT32 i;
	CACHE_GLYPH_V2_ORDER* cache_glyph_v2 = calloc(1, sizeof(CACHE_GLYPH_V2_ORDER));

	if (!cache_glyph_v2)
		goto fail;

	cache_glyph_v2->cacheId = (flags & 0x000F);
	cache_glyph_v2->flags = (flags & 0x00F0) >> 4;
	cache_glyph_v2->cGlyphs = (flags & 0xFF00) >> 8;

	for (i = 0; i < cache_glyph_v2->cGlyphs; i++)
	{
		GLYPH_DATA_V2* glyph = &cache_glyph_v2->glyphData[i];

		if (Stream_GetRemainingLength(s) < 1)
			goto fail;

		Stream_Read_UINT8(s, glyph->cacheIndex);

		if (!update_read_2byte_signed(s, &glyph->x) || !update_read_2byte_signed(s, &glyph->y) ||
		    !update_read_2byte_unsigned(s, &glyph->cx) ||
		    !update_read_2byte_unsigned(s, &glyph->cy))
		{
			goto fail;
		}

		glyph->cb = ((glyph->cx + 7) / 8) * glyph->cy;
		glyph->cb += ((glyph->cb % 4) > 0) ? 4 - (glyph->cb % 4) : 0;

		if (Stream_GetRemainingLength(s) < glyph->cb)
			goto fail;

		glyph->aj = (BYTE*)malloc(glyph->cb);

		if (!glyph->aj)
			goto fail;

		Stream_Read(s, glyph->aj, glyph->cb);
	}

	if ((flags & CG_GLYPH_UNICODE_PRESENT) && (cache_glyph_v2->cGlyphs > 0))
	{
		cache_glyph_v2->unicodeCharacters = calloc(cache_glyph_v2->cGlyphs, sizeof(WCHAR));

		if (!cache_glyph_v2->unicodeCharacters)
			goto fail;

		if (Stream_GetRemainingLength(s) < sizeof(WCHAR) * cache_glyph_v2->cGlyphs)
			goto fail;

		Stream_Read_UTF16_String(s, cache_glyph_v2->unicodeCharacters, cache_glyph_v2->cGlyphs);
	}

	return cache_glyph_v2;
fail:
	free_cache_glyph_v2_order(update->context, cache_glyph_v2);
	return NULL;
}