static INLINE BOOL update_read_2byte_unsigned(wStream* s, UINT32* value)
{
	BYTE byte;

	if (Stream_GetRemainingLength(s) < 1)
		return FALSE;

	Stream_Read_UINT8(s, byte);

	if (byte & 0x80)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		*value = (byte & 0x7F) << 8;
		Stream_Read_UINT8(s, byte);
		*value |= byte;
	}
	else
	{
		*value = (byte & 0x7F);
	}

	return TRUE;
}