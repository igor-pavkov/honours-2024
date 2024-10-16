static BOOL update_read_polygon_sc_order(wStream* s, const ORDER_INFO* orderInfo,
                                         POLYGON_SC_ORDER* polygon_sc)
{
	UINT32 num = polygon_sc->numPoints;
	ORDER_FIELD_COORD(1, polygon_sc->xStart);
	ORDER_FIELD_COORD(2, polygon_sc->yStart);
	ORDER_FIELD_BYTE(3, polygon_sc->bRop2);
	ORDER_FIELD_BYTE(4, polygon_sc->fillMode);
	ORDER_FIELD_COLOR(orderInfo, s, 5, &polygon_sc->brushColor);
	ORDER_FIELD_BYTE(6, num);

	if (orderInfo->fieldFlags & ORDER_FIELD_07)
	{
		DELTA_POINT* newpoints;

		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, polygon_sc->cbData);
		newpoints = (DELTA_POINT*)realloc(polygon_sc->points, sizeof(DELTA_POINT) * num);

		if (!newpoints)
			return FALSE;

		polygon_sc->points = newpoints;
		polygon_sc->numPoints = num;
		return update_read_delta_points(s, polygon_sc->points, polygon_sc->numPoints,
		                                polygon_sc->xStart, polygon_sc->yStart);
	}

	return TRUE;
}