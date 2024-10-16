static BOOL update_read_polygon_cb_order(wStream* s, const ORDER_INFO* orderInfo,
                                         POLYGON_CB_ORDER* polygon_cb)
{
	UINT32 num = polygon_cb->numPoints;
	ORDER_FIELD_COORD(1, polygon_cb->xStart);
	ORDER_FIELD_COORD(2, polygon_cb->yStart);
	ORDER_FIELD_BYTE(3, polygon_cb->bRop2);
	ORDER_FIELD_BYTE(4, polygon_cb->fillMode);
	ORDER_FIELD_COLOR(orderInfo, s, 5, &polygon_cb->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 6, &polygon_cb->foreColor);

	if (!update_read_brush(s, &polygon_cb->brush, orderInfo->fieldFlags >> 6))
		return FALSE;

	ORDER_FIELD_BYTE(12, num);

	if (orderInfo->fieldFlags & ORDER_FIELD_13)
	{
		DELTA_POINT* newpoints;

		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, polygon_cb->cbData);
		newpoints = (DELTA_POINT*)realloc(polygon_cb->points, sizeof(DELTA_POINT) * num);

		if (!newpoints)
			return FALSE;

		polygon_cb->points = newpoints;
		polygon_cb->numPoints = num;

		if (!update_read_delta_points(s, polygon_cb->points, polygon_cb->numPoints,
		                              polygon_cb->xStart, polygon_cb->yStart))
			return FALSE;
	}

	polygon_cb->backMode = (polygon_cb->bRop2 & 0x80) ? BACKMODE_TRANSPARENT : BACKMODE_OPAQUE;
	polygon_cb->bRop2 = (polygon_cb->bRop2 & 0x1F);
	return TRUE;
}