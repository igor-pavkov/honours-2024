BOOL update_write_dstblt_order(wStream* s, ORDER_INFO* orderInfo, const DSTBLT_ORDER* dstblt)
{
	if (!Stream_EnsureRemainingCapacity(s, update_approximate_dstblt_order(orderInfo, dstblt)))
		return FALSE;

	orderInfo->fieldFlags = 0;
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	update_write_coord(s, dstblt->nLeftRect);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	update_write_coord(s, dstblt->nTopRect);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	update_write_coord(s, dstblt->nWidth);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	update_write_coord(s, dstblt->nHeight);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	Stream_Write_UINT8(s, dstblt->bRop);
	return TRUE;
}