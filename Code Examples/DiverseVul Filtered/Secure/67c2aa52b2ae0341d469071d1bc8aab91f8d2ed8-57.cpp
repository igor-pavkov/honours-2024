static BOOL update_read_line_to_order(wStream* s, const ORDER_INFO* orderInfo,
                                      LINE_TO_ORDER* line_to)
{
	ORDER_FIELD_UINT16(1, line_to->backMode);
	ORDER_FIELD_COORD(2, line_to->nXStart);
	ORDER_FIELD_COORD(3, line_to->nYStart);
	ORDER_FIELD_COORD(4, line_to->nXEnd);
	ORDER_FIELD_COORD(5, line_to->nYEnd);
	ORDER_FIELD_COLOR(orderInfo, s, 6, &line_to->backColor);
	ORDER_FIELD_BYTE(7, line_to->bRop2);
	ORDER_FIELD_BYTE(8, line_to->penStyle);
	ORDER_FIELD_BYTE(9, line_to->penWidth);
	ORDER_FIELD_COLOR(orderInfo, s, 10, &line_to->penColor);
	return TRUE;
}