BOOL update_write_field_flags(wStream* s, UINT32 fieldFlags, BYTE flags, BYTE fieldBytes)
{
	BYTE byte;

	if (fieldBytes == 1)
	{
		byte = fieldFlags & 0xFF;
		Stream_Write_UINT8(s, byte);
	}
	else if (fieldBytes == 2)
	{
		byte = fieldFlags & 0xFF;
		Stream_Write_UINT8(s, byte);
		byte = (fieldFlags >> 8) & 0xFF;
		Stream_Write_UINT8(s, byte);
	}
	else if (fieldBytes == 3)
	{
		byte = fieldFlags & 0xFF;
		Stream_Write_UINT8(s, byte);
		byte = (fieldFlags >> 8) & 0xFF;
		Stream_Write_UINT8(s, byte);
		byte = (fieldFlags >> 16) & 0xFF;
		Stream_Write_UINT8(s, byte);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}