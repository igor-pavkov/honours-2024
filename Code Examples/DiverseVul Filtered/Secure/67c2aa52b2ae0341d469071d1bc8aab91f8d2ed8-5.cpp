BOOL update_write_patblt_order(wStream* s, ORDER_INFO* orderInfo, PATBLT_ORDER* patblt)
{
	if (!Stream_EnsureRemainingCapacity(s, update_approximate_patblt_order(orderInfo, patblt)))
		return FALSE;

	orderInfo->fieldFlags = 0;
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	update_write_coord(s, patblt->nLeftRect);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	update_write_coord(s, patblt->nTopRect);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	update_write_coord(s, patblt->nWidth);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	update_write_coord(s, patblt->nHeight);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	Stream_Write_UINT8(s, patblt->bRop);
	orderInfo->fieldFlags |= ORDER_FIELD_06;
	update_write_color(s, patblt->backColor);
	orderInfo->fieldFlags |= ORDER_FIELD_07;
	update_write_color(s, patblt->foreColor);
	orderInfo->fieldFlags |= ORDER_FIELD_08;
	orderInfo->fieldFlags |= ORDER_FIELD_09;
	orderInfo->fieldFlags |= ORDER_FIELD_10;
	orderInfo->fieldFlags |= ORDER_FIELD_11;
	orderInfo->fieldFlags |= ORDER_FIELD_12;
	update_write_brush(s, &patblt->brush, orderInfo->fieldFlags >> 7);
	return TRUE;
}