static INLINE BOOL update_read_color(wStream* s, UINT32* color)
{
	BYTE byte;

	if (Stream_GetRemainingLength(s) < 3)
		return FALSE;

	*color = 0;
	Stream_Read_UINT8(s, byte);
	*color = (UINT32)byte;
	Stream_Read_UINT8(s, byte);
	*color |= ((UINT32)byte << 8) & 0xFF00;
	Stream_Read_UINT8(s, byte);
	*color |= ((UINT32)byte << 16) & 0xFF0000;
	return TRUE;
}