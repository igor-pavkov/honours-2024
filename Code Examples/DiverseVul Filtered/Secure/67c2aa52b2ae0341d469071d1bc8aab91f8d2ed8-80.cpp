static BOOL update_read_patblt_order(wStream* s, const ORDER_INFO* orderInfo, PATBLT_ORDER* patblt)
{
	ORDER_FIELD_COORD(1, patblt->nLeftRect);
	ORDER_FIELD_COORD(2, patblt->nTopRect);
	ORDER_FIELD_COORD(3, patblt->nWidth);
	ORDER_FIELD_COORD(4, patblt->nHeight);
	ORDER_FIELD_BYTE(5, patblt->bRop);
	ORDER_FIELD_COLOR(orderInfo, s, 6, &patblt->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 7, &patblt->foreColor);
	return update_read_brush(s, &patblt->brush, orderInfo->fieldFlags >> 7);
}