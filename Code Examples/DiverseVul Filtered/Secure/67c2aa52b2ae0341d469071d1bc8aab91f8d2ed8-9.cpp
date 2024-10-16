static BOOL update_read_ellipse_cb_order(wStream* s, const ORDER_INFO* orderInfo,
                                         ELLIPSE_CB_ORDER* ellipse_cb)
{
	ORDER_FIELD_COORD(1, ellipse_cb->leftRect);
	ORDER_FIELD_COORD(2, ellipse_cb->topRect);
	ORDER_FIELD_COORD(3, ellipse_cb->rightRect);
	ORDER_FIELD_COORD(4, ellipse_cb->bottomRect);
	ORDER_FIELD_BYTE(5, ellipse_cb->bRop2);
	ORDER_FIELD_BYTE(6, ellipse_cb->fillMode);
	ORDER_FIELD_COLOR(orderInfo, s, 7, &ellipse_cb->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 8, &ellipse_cb->foreColor);
	return update_read_brush(s, &ellipse_cb->brush, orderInfo->fieldFlags >> 8);
}