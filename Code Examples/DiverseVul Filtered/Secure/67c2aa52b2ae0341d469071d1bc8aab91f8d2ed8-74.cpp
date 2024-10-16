static INLINE BOOL update_read_colorref(wStream* s, UINT32* color)
{
	BYTE byte;

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	*color = 0;
	Stream_Read_UINT8(s, byte);
	*color = byte;
	Stream_Read_UINT8(s, byte);
	*color |= ((UINT32)byte << 8);
	Stream_Read_UINT8(s, byte);
	*color |= ((UINT32)byte << 16);
	Stream_Seek_UINT8(s);
	return TRUE;
}