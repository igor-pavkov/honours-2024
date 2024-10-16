static INLINE void update_seek_glyph_delta(wStream* s)
{
	BYTE byte;
	Stream_Read_UINT8(s, byte);

	if (byte & 0x80)
		Stream_Seek_UINT8(s);
}