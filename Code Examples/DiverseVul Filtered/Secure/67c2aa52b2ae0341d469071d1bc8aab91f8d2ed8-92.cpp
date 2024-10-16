static BOOL update_read_polyline_order(wStream* s, const ORDER_INFO* orderInfo,
                                       POLYLINE_ORDER* polyline)
{
	UINT16 word;
	UINT32 new_num = polyline->numDeltaEntries;
	ORDER_FIELD_COORD(1, polyline->xStart);
	ORDER_FIELD_COORD(2, polyline->yStart);
	ORDER_FIELD_BYTE(3, polyline->bRop2);
	ORDER_FIELD_UINT16(4, word);
	ORDER_FIELD_COLOR(orderInfo, s, 5, &polyline->penColor);
	ORDER_FIELD_BYTE(6, new_num);

	if (orderInfo->fieldFlags & ORDER_FIELD_07)
	{
		DELTA_POINT* new_points;

		if (Stream_GetRemainingLength(s) < 1)
		{
			WLog_ERR(TAG, "Stream_GetRemainingLength(s) < 1");
			return FALSE;
		}

		Stream_Read_UINT8(s, polyline->cbData);
		new_points = (DELTA_POINT*)realloc(polyline->points, sizeof(DELTA_POINT) * new_num);

		if (!new_points)
		{
			WLog_ERR(TAG, "realloc(%" PRIu32 ") failed", new_num);
			return FALSE;
		}

		polyline->points = new_points;
		polyline->numDeltaEntries = new_num;
		return update_read_delta_points(s, polyline->points, polyline->numDeltaEntries,
		                                polyline->xStart, polyline->yStart);
	}

	return TRUE;
}