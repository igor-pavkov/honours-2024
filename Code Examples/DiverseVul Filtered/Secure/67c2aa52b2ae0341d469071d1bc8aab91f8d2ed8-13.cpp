BOOL update_write_scrblt_order(wStream* s, ORDER_INFO* orderInfo, const SCRBLT_ORDER* scrblt)
{
	if (!Stream_EnsureRemainingCapacity(s, update_approximate_scrblt_order(orderInfo, scrblt)))
		return FALSE;

	orderInfo->fieldFlags = 0;
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	update_write_coord(s, scrblt->nLeftRect);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	update_write_coord(s, scrblt->nTopRect);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	update_write_coord(s, scrblt->nWidth);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	update_write_coord(s, scrblt->nHeight);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	Stream_Write_UINT8(s, scrblt->bRop);
	orderInfo->fieldFlags |= ORDER_FIELD_06;
	update_write_coord(s, scrblt->nXSrc);
	orderInfo->fieldFlags |= ORDER_FIELD_07;
	update_write_coord(s, scrblt->nYSrc);
	return TRUE;
}