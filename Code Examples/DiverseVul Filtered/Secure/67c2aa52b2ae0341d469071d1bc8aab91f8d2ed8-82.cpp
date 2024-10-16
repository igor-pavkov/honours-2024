static BOOL update_read_opaque_rect_order(wStream* s, const ORDER_INFO* orderInfo,
                                          OPAQUE_RECT_ORDER* opaque_rect)
{
	BYTE byte;
	ORDER_FIELD_COORD(1, opaque_rect->nLeftRect);
	ORDER_FIELD_COORD(2, opaque_rect->nTopRect);
	ORDER_FIELD_COORD(3, opaque_rect->nWidth);
	ORDER_FIELD_COORD(4, opaque_rect->nHeight);

	if (orderInfo->fieldFlags & ORDER_FIELD_05)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, byte);
		opaque_rect->color = (opaque_rect->color & 0x00FFFF00) | ((UINT32)byte);
	}

	if (orderInfo->fieldFlags & ORDER_FIELD_06)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, byte);
		opaque_rect->color = (opaque_rect->color & 0x00FF00FF) | ((UINT32)byte << 8);
	}

	if (orderInfo->fieldFlags & ORDER_FIELD_07)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, byte);
		opaque_rect->color = (opaque_rect->color & 0x0000FFFF) | ((UINT32)byte << 16);
	}

	return TRUE;
}