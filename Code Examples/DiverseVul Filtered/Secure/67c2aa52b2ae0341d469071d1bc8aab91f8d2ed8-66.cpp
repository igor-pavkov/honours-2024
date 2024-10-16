static BOOL update_read_fast_glyph_order(wStream* s, const ORDER_INFO* orderInfo,
                                         FAST_GLYPH_ORDER* fastGlyph)
{
	BYTE* phold;
	GLYPH_DATA_V2* glyph = &fastGlyph->glyphData;
	ORDER_FIELD_BYTE(1, fastGlyph->cacheId);
	ORDER_FIELD_2BYTE(2, fastGlyph->ulCharInc, fastGlyph->flAccel);
	ORDER_FIELD_COLOR(orderInfo, s, 3, &fastGlyph->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 4, &fastGlyph->foreColor);
	ORDER_FIELD_COORD(5, fastGlyph->bkLeft);
	ORDER_FIELD_COORD(6, fastGlyph->bkTop);
	ORDER_FIELD_COORD(7, fastGlyph->bkRight);
	ORDER_FIELD_COORD(8, fastGlyph->bkBottom);
	ORDER_FIELD_COORD(9, fastGlyph->opLeft);
	ORDER_FIELD_COORD(10, fastGlyph->opTop);
	ORDER_FIELD_COORD(11, fastGlyph->opRight);
	ORDER_FIELD_COORD(12, fastGlyph->opBottom);
	ORDER_FIELD_COORD(13, fastGlyph->x);
	ORDER_FIELD_COORD(14, fastGlyph->y);

	if (orderInfo->fieldFlags & ORDER_FIELD_15)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, fastGlyph->cbData);

		if (Stream_GetRemainingLength(s) < fastGlyph->cbData)
			return FALSE;

		CopyMemory(fastGlyph->data, Stream_Pointer(s), fastGlyph->cbData);
		phold = Stream_Pointer(s);

		if (!Stream_SafeSeek(s, 1))
			return FALSE;

		if (fastGlyph->cbData > 1)
		{
			UINT32 new_cb;
			/* parse optional glyph data */
			glyph->cacheIndex = fastGlyph->data[0];

			if (!update_read_2byte_signed(s, &glyph->x) ||
			    !update_read_2byte_signed(s, &glyph->y) ||
			    !update_read_2byte_unsigned(s, &glyph->cx) ||
			    !update_read_2byte_unsigned(s, &glyph->cy))
				return FALSE;

			glyph->cb = ((glyph->cx + 7) / 8) * glyph->cy;
			glyph->cb += ((glyph->cb % 4) > 0) ? 4 - (glyph->cb % 4) : 0;
			new_cb = ((glyph->cx + 7) / 8) * glyph->cy;
			new_cb += ((new_cb % 4) > 0) ? 4 - (new_cb % 4) : 0;

			if (Stream_GetRemainingLength(s) < new_cb)
				return FALSE;

			if (new_cb)
			{
				BYTE* new_aj;
				new_aj = (BYTE*)realloc(glyph->aj, new_cb);

				if (!new_aj)
					return FALSE;

				glyph->aj = new_aj;
				glyph->cb = new_cb;
				Stream_Read(s, glyph->aj, glyph->cb);
			}
		}

		Stream_SetPointer(s, phold + fastGlyph->cbData);
	}

	return TRUE;
}