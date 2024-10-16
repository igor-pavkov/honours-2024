static INLINE void update_read_glyph_delta(wStream* s, UINT16* value)
{
	BYTE byte;
	Stream_Read_UINT8(s, byte);

	if (byte == 0x80)
		Stream_Read_UINT16(s, *value);
	else
		*value = (byte & 0x3F);
}