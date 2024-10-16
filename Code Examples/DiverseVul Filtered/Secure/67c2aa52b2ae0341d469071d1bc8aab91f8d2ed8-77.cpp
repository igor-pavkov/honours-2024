BOOL update_write_cache_glyph_v2_order(wStream* s, const CACHE_GLYPH_V2_ORDER* cache_glyph_v2,
                                       UINT16* flags)
{
	UINT32 i, inf;
	inf = update_approximate_cache_glyph_v2_order(cache_glyph_v2, flags);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	*flags = (cache_glyph_v2->cacheId & 0x000F) | ((cache_glyph_v2->flags & 0x000F) << 4) |
	         ((cache_glyph_v2->cGlyphs & 0x00FF) << 8);

	for (i = 0; i < cache_glyph_v2->cGlyphs; i++)
	{
		UINT32 cb;
		const GLYPH_DATA_V2* glyph = &cache_glyph_v2->glyphData[i];
		Stream_Write_UINT8(s, glyph->cacheIndex);

		if (!update_write_2byte_signed(s, glyph->x) || !update_write_2byte_signed(s, glyph->y) ||
		    !update_write_2byte_unsigned(s, glyph->cx) ||
		    !update_write_2byte_unsigned(s, glyph->cy))
		{
			return FALSE;
		}

		cb = ((glyph->cx + 7) / 8) * glyph->cy;
		cb += ((cb % 4) > 0) ? 4 - (cb % 4) : 0;
		Stream_Write(s, glyph->aj, cb);
	}

	if (*flags & CG_GLYPH_UNICODE_PRESENT)
	{
		Stream_Zero(s, cache_glyph_v2->cGlyphs * 2);
	}

	return TRUE;
}