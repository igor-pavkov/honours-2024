static INLINE BOOL update_write_2byte_unsigned(wStream* s, UINT32 value)
{
	BYTE byte;

	if (value > 0x7FFF)
		return FALSE;

	if (value >= 0x7F)
	{
		byte = ((value & 0x7F00) >> 8);
		Stream_Write_UINT8(s, byte | 0x80);
		byte = (value & 0xFF);
		Stream_Write_UINT8(s, byte);
	}
	else
	{
		byte = (value & 0x7F);
		Stream_Write_UINT8(s, byte);
	}

	return TRUE;
}