BOOL update_write_line_to_order(wStream* s, ORDER_INFO* orderInfo, const LINE_TO_ORDER* line_to)
{
	if (!Stream_EnsureRemainingCapacity(s, update_approximate_line_to_order(orderInfo, line_to)))
		return FALSE;

	orderInfo->fieldFlags = 0;
	orderInfo->fieldFlags |= ORDER_FIELD_01;
	Stream_Write_UINT16(s, line_to->backMode);
	orderInfo->fieldFlags |= ORDER_FIELD_02;
	update_write_coord(s, line_to->nXStart);
	orderInfo->fieldFlags |= ORDER_FIELD_03;
	update_write_coord(s, line_to->nYStart);
	orderInfo->fieldFlags |= ORDER_FIELD_04;
	update_write_coord(s, line_to->nXEnd);
	orderInfo->fieldFlags |= ORDER_FIELD_05;
	update_write_coord(s, line_to->nYEnd);
	orderInfo->fieldFlags |= ORDER_FIELD_06;
	update_write_color(s, line_to->backColor);
	orderInfo->fieldFlags |= ORDER_FIELD_07;
	Stream_Write_UINT8(s, line_to->bRop2);
	orderInfo->fieldFlags |= ORDER_FIELD_08;
	Stream_Write_UINT8(s, line_to->penStyle);
	orderInfo->fieldFlags |= ORDER_FIELD_09;
	Stream_Write_UINT8(s, line_to->penWidth);
	orderInfo->fieldFlags |= ORDER_FIELD_10;
	update_write_color(s, line_to->penColor);
	return TRUE;
}