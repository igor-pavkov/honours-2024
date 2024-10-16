static BOOL update_read_fast_index_order(wStream* s, const ORDER_INFO* orderInfo,
                                         FAST_INDEX_ORDER* fast_index)
{
	ORDER_FIELD_BYTE(1, fast_index->cacheId);
	ORDER_FIELD_2BYTE(2, fast_index->ulCharInc, fast_index->flAccel);
	ORDER_FIELD_COLOR(orderInfo, s, 3, &fast_index->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 4, &fast_index->foreColor);
	ORDER_FIELD_COORD(5, fast_index->bkLeft);
	ORDER_FIELD_COORD(6, fast_index->bkTop);
	ORDER_FIELD_COORD(7, fast_index->bkRight);
	ORDER_FIELD_COORD(8, fast_index->bkBottom);
	ORDER_FIELD_COORD(9, fast_index->opLeft);
	ORDER_FIELD_COORD(10, fast_index->opTop);
	ORDER_FIELD_COORD(11, fast_index->opRight);
	ORDER_FIELD_COORD(12, fast_index->opBottom);
	ORDER_FIELD_COORD(13, fast_index->x);
	ORDER_FIELD_COORD(14, fast_index->y);

	if (orderInfo->fieldFlags & ORDER_FIELD_15)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, fast_index->cbData);

		if (Stream_GetRemainingLength(s) < fast_index->cbData)
			return FALSE;

		CopyMemory(fast_index->data, Stream_Pointer(s), fast_index->cbData);
		Stream_Seek(s, fast_index->cbData);
	}

	return TRUE;
}