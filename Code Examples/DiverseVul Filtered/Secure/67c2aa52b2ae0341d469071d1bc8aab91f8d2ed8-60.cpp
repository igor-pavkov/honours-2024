static INLINE BOOL update_read_2byte_signed(wStream* s, INT32* value)
{
	BYTE byte;
	BOOL negative;

	if (Stream_GetRemainingLength(s) < 1)
		return FALSE;

	Stream_Read_UINT8(s, byte);
	negative = (byte & 0x40) ? TRUE : FALSE;
	*value = (byte & 0x3F);

	if (byte & 0x80)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, byte);
		*value = (*value << 8) | byte;
	}

	if (negative)
		*value *= -1;

	return TRUE;
}