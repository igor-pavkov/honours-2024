static BOOL update_read_scrblt_order(wStream* s, const ORDER_INFO* orderInfo, SCRBLT_ORDER* scrblt)
{
	ORDER_FIELD_COORD(1, scrblt->nLeftRect);
	ORDER_FIELD_COORD(2, scrblt->nTopRect);
	ORDER_FIELD_COORD(3, scrblt->nWidth);
	ORDER_FIELD_COORD(4, scrblt->nHeight);
	ORDER_FIELD_BYTE(5, scrblt->bRop);
	ORDER_FIELD_COORD(6, scrblt->nXSrc);
	ORDER_FIELD_COORD(7, scrblt->nYSrc);
	return TRUE;
}