static INLINE BOOL update_read_4byte_unsigned(wStream* s, UINT32* value)
{
	BYTE byte;
	BYTE count;

	if (Stream_GetRemainingLength(s) < 1)
		return FALSE;

	Stream_Read_UINT8(s, byte);
	count = (byte & 0xC0) >> 6;

	if (Stream_GetRemainingLength(s) < count)
		return FALSE;

	switch (count)
	{
		case 0:
			*value = (byte & 0x3F);
			break;

		case 1:
			*value = (byte & 0x3F) << 8;
			Stream_Read_UINT8(s, byte);
			*value |= byte;
			break;

		case 2:
			*value = (byte & 0x3F) << 16;
			Stream_Read_UINT8(s, byte);
			*value |= (byte << 8);
			Stream_Read_UINT8(s, byte);
			*value |= byte;
			break;

		case 3:
			*value = (byte & 0x3F) << 24;
			Stream_Read_UINT8(s, byte);
			*value |= (byte << 16);
			Stream_Read_UINT8(s, byte);
			*value |= (byte << 8);
			Stream_Read_UINT8(s, byte);
			*value |= byte;
			break;

		default:
			break;
	}

	return TRUE;
}