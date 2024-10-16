BOOL update_write_memblt_order(wStream* s, ORDER_INFO* orderInfo, const MEMBLT_ORDER* memblt)
{
	UINT16 cacheId;

	if (!Stream_EnsureRemainingCapacity(s, update_approximate_memblt_order(orderInfo, memblt)))
		return FALSE;

	cacheId = (memblt->cacheId & 0xFF) | ((memblt->colorIndex & 0xFF) << 8);
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	Stream_Write_UINT16(s, cacheId);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	update_write_coord(s, memblt->nLeftRect);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	update_write_coord(s, memblt->nTopRect);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	update_write_coord(s, memblt->nWidth);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	update_write_coord(s, memblt->nHeight);
	orderInfo->fieldFlags |= ORDER_FIELD_06;
	Stream_Write_UINT8(s, memblt->bRop);
	orderInfo->fieldFlags |= ORDER_FIELD_07;
	update_write_coord(s, memblt->nXSrc);
	orderInfo->fieldFlags |= ORDER_FIELD_08;
	update_write_coord(s, memblt->nYSrc);
	orderInfo->fieldFlags |= ORDER_FIELD_09;
	Stream_Write_UINT16(s, memblt->cacheIndex);
	return TRUE;
}