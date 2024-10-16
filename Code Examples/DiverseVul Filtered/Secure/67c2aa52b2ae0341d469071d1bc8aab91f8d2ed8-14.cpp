static BOOL update_read_dstblt_order(wStream* s, const ORDER_INFO* orderInfo, DSTBLT_ORDER* dstblt)
{
	ORDER_FIELD_COORD(1, dstblt->nLeftRect);
	ORDER_FIELD_COORD(2, dstblt->nTopRect);
	ORDER_FIELD_COORD(3, dstblt->nWidth);
	ORDER_FIELD_COORD(4, dstblt->nHeight);
	ORDER_FIELD_BYTE(5, dstblt->bRop);
	return TRUE;
}