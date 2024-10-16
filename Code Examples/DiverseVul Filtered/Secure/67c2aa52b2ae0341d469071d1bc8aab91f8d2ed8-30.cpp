static BOOL update_read_glyph_index_order(wStream* s, const ORDER_INFO* orderInfo,
                                          GLYPH_INDEX_ORDER* glyph_index)
{
	ORDER_FIELD_BYTE(1, glyph_index->cacheId);
	ORDER_FIELD_BYTE(2, glyph_index->flAccel);
	ORDER_FIELD_BYTE(3, glyph_index->ulCharInc);
	ORDER_FIELD_BYTE(4, glyph_index->fOpRedundant);
	ORDER_FIELD_COLOR(orderInfo, s, 5, &glyph_index->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 6, &glyph_index->foreColor);
	ORDER_FIELD_UINT16(7, glyph_index->bkLeft);
	ORDER_FIELD_UINT16(8, glyph_index->bkTop);
	ORDER_FIELD_UINT16(9, glyph_index->bkRight);
	ORDER_FIELD_UINT16(10, glyph_index->bkBottom);
	ORDER_FIELD_UINT16(11, glyph_index->opLeft);
	ORDER_FIELD_UINT16(12, glyph_index->opTop);
	ORDER_FIELD_UINT16(13, glyph_index->opRight);
	ORDER_FIELD_UINT16(14, glyph_index->opBottom);

	if (!update_read_brush(s, &glyph_index->brush, orderInfo->fieldFlags >> 14))
		return FALSE;

	ORDER_FIELD_UINT16(20, glyph_index->x);
	ORDER_FIELD_UINT16(21, glyph_index->y);

	if (orderInfo->fieldFlags & ORDER_FIELD_22)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, glyph_index->cbData);

		if (Stream_GetRemainingLength(s) < glyph_index->cbData)
			return FALSE;

		CopyMemory(glyph_index->data, Stream_Pointer(s), glyph_index->cbData);
		Stream_Seek(s, glyph_index->cbData);
	}

	return TRUE;
}