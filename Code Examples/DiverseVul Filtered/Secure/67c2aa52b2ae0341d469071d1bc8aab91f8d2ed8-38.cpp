static INLINE BOOL update_write_4byte_unsigned(wStream* s, UINT32 value)
{
	BYTE byte;

	if (value <= 0x3F)
	{
		Stream_Write_UINT8(s, value);
	}
	else if (value <= 0x3FFF)
	{
		byte = (value >> 8) & 0x3F;
		Stream_Write_UINT8(s, byte | 0x40);
		byte = (value & 0xFF);
		Stream_Write_UINT8(s, byte);
	}
	else if (value <= 0x3FFFFF)
	{
		byte = (value >> 16) & 0x3F;
		Stream_Write_UINT8(s, byte | 0x80);
		byte = (value >> 8) & 0xFF;
		Stream_Write_UINT8(s, byte);
		byte = (value & 0xFF);
		Stream_Write_UINT8(s, byte);
	}
	else if (value <= 0x3FFFFFFF)
	{
		byte = (value >> 24) & 0x3F;
		Stream_Write_UINT8(s, byte | 0xC0);
		byte = (value >> 16) & 0xFF;
		Stream_Write_UINT8(s, byte);
		byte = (value >> 8) & 0xFF;
		Stream_Write_UINT8(s, byte);
		byte = (value & 0xFF);
		Stream_Write_UINT8(s, byte);
	}
	else
		return FALSE;

	return TRUE;
}