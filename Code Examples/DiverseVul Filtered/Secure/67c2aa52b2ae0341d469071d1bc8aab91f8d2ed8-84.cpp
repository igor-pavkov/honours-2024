static INLINE void update_write_color_quad(wStream* s, UINT32 color)
{
	BYTE byte;
	byte = (color >> 16) & 0xFF;
	Stream_Write_UINT8(s, byte);
	byte = (color >> 8) & 0xFF;
	Stream_Write_UINT8(s, byte);
	byte = color & 0xFF;
	Stream_Write_UINT8(s, byte);
}