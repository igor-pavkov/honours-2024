BOOL update_write_glyph_index_order(wStream* s, ORDER_INFO* orderInfo,
                                    GLYPH_INDEX_ORDER* glyph_index)
{
	int inf = update_approximate_glyph_index_order(orderInfo, glyph_index);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	orderInfo->fieldFlags = 0;
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	Stream_Write_UINT8(s, glyph_index->cacheId);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	Stream_Write_UINT8(s, glyph_index->flAccel);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	Stream_Write_UINT8(s, glyph_index->ulCharInc);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	Stream_Write_UINT8(s, glyph_index->fOpRedundant);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	update_write_color(s, glyph_index->backColor);
	orderInfo->fieldFlags |= ORDER_FIELD_06;
	update_write_color(s, glyph_index->foreColor);
	orderInfo->fieldFlags |= ORDER_FIELD_07;
	Stream_Write_UINT16(s, glyph_index->bkLeft);
	orderInfo->fieldFlags |= ORDER_FIELD_08;
	Stream_Write_UINT16(s, glyph_index->bkTop);
	orderInfo->fieldFlags |= ORDER_FIELD_09;
	Stream_Write_UINT16(s, glyph_index->bkRight);
	orderInfo->fieldFlags |= ORDER_FIELD_10;
	Stream_Write_UINT16(s, glyph_index->bkBottom);
	orderInfo->fieldFlags |= ORDER_FIELD_11;
	Stream_Write_UINT16(s, glyph_index->opLeft);
	orderInfo->fieldFlags |= ORDER_FIELD_12;
	Stream_Write_UINT16(s, glyph_index->opTop);
	orderInfo->fieldFlags |= ORDER_FIELD_13;
	Stream_Write_UINT16(s, glyph_index->opRight);
	orderInfo->fieldFlags |= ORDER_FIELD_14;
	Stream_Write_UINT16(s, glyph_index->opBottom);
	orderInfo->fieldFlags |= ORDER_FIELD_15;
	orderInfo->fieldFlags |= ORDER_FIELD_16;
	orderInfo->fieldFlags |= ORDER_FIELD_17;
	orderInfo->fieldFlags |= ORDER_FIELD_18;
	orderInfo->fieldFlags |= ORDER_FIELD_19;
	update_write_brush(s, &glyph_index->brush, orderInfo->fieldFlags >> 14);
	orderInfo->fieldFlags |= ORDER_FIELD_20;
	Stream_Write_UINT16(s, glyph_index->x);
	orderInfo->fieldFlags |= ORDER_FIELD_21;
	Stream_Write_UINT16(s, glyph_index->y);
	orderInfo->fieldFlags |= ORDER_FIELD_22;
	Stream_Write_UINT8(s, glyph_index->cbData);
	Stream_Write(s, glyph_index->data, glyph_index->cbData);
	return TRUE;
}