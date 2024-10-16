static INLINE BOOL update_read_delta_points(wStream* s, DELTA_POINT* points, int number, INT16 x,
                                            INT16 y)
{
	int i;
	BYTE flags = 0;
	BYTE* zeroBits;
	UINT32 zeroBitsSize;
	zeroBitsSize = ((number + 3) / 4);

	if (Stream_GetRemainingLength(s) < zeroBitsSize)
	{
		WLog_ERR(TAG, "Stream_GetRemainingLength(s) < %" PRIu32 "", zeroBitsSize);
		return FALSE;
	}

	Stream_GetPointer(s, zeroBits);
	Stream_Seek(s, zeroBitsSize);
	ZeroMemory(points, sizeof(DELTA_POINT) * number);

	for (i = 0; i < number; i++)
	{
		if (i % 4 == 0)
			flags = zeroBits[i / 4];

		if ((~flags & 0x80) && !update_read_delta(s, &points[i].x))
		{
			WLog_ERR(TAG, "update_read_delta(x) failed");
			return FALSE;
		}

		if ((~flags & 0x40) && !update_read_delta(s, &points[i].y))
		{
			WLog_ERR(TAG, "update_read_delta(y) failed");
			return FALSE;
		}

		flags <<= 2;
	}

	return TRUE;
}