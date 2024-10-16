static INLINE BOOL update_read_coord(wStream* s, INT32* coord, BOOL delta)
{
	INT8 lsi8;
	INT16 lsi16;

	if (delta)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_INT8(s, lsi8);
		*coord += lsi8;
	}
	else
	{
		if (Stream_GetRemainingLength(s) < 2)
			return FALSE;

		Stream_Read_INT16(s, lsi16);
		*coord = lsi16;
	}

	return TRUE;
}