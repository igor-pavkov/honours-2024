BOOL update_write_opaque_rect_order(wStream* s, ORDER_INFO* orderInfo,
                                    const OPAQUE_RECT_ORDER* opaque_rect)
{
	BYTE byte;
	int inf = update_approximate_opaque_rect_order(orderInfo, opaque_rect);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	// TODO: Color format conversion
	orderInfo->fieldFlags = 0;
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	update_write_coord(s, opaque_rect->nLeftRect);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	update_write_coord(s, opaque_rect->nTopRect);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	update_write_coord(s, opaque_rect->nWidth);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	update_write_coord(s, opaque_rect->nHeight);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	byte = opaque_rect->color & 0x000000FF;
	Stream_Write_UINT8(s, byte);
	orderInfo->fieldFlags |= ORDER_FIELD_06;
	byte = (opaque_rect->color & 0x0000FF00) >> 8;
	Stream_Write_UINT8(s, byte);
	orderInfo->fieldFlags |= ORDER_FIELD_07;
	byte = (opaque_rect->color & 0x00FF0000) >> 16;
	Stream_Write_UINT8(s, byte);
	return TRUE;
}