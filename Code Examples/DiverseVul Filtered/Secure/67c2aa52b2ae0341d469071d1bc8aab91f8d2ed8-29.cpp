static INLINE BOOL update_write_2byte_signed(wStream* s, INT32 value)
{
	BYTE byte;
	BOOL negative = FALSE;

	if (value < 0)
	{
		negative = TRUE;
		value *= -1;
	}

	if (value > 0x3FFF)
		return FALSE;

	if (value >= 0x3F)
	{
		byte = ((value & 0x3F00) >> 8);

		if (negative)
			byte |= 0x40;

		Stream_Write_UINT8(s, byte | 0x80);
		byte = (value & 0xFF);
		Stream_Write_UINT8(s, byte);
	}
	else
	{
		byte = (value & 0x3F);

		if (negative)
			byte |= 0x40;

		Stream_Write_UINT8(s, byte);
	}

	return TRUE;
}