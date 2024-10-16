static INLINE BOOL update_read_delta(wStream* s, INT32* value)
{
	BYTE byte;

	if (Stream_GetRemainingLength(s) < 1)
	{
		WLog_ERR(TAG, "Stream_GetRemainingLength(s) < 1");
		return FALSE;
	}

	Stream_Read_UINT8(s, byte);

	if (byte & 0x40)
		*value = (byte | ~0x3F);
	else
		*value = (byte & 0x3F);

	if (byte & 0x80)
	{
		if (Stream_GetRemainingLength(s) < 1)
		{
			WLog_ERR(TAG, "Stream_GetRemainingLength(s) < 1");
			return FALSE;
		}

		Stream_Read_UINT8(s, byte);
		*value = (*value << 8) | byte;
	}

	return TRUE;
}