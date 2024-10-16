UINT rdpgfx_write_point16(wStream* s, const RDPGFX_POINT16* point16)
{
	Stream_Write_UINT16(s, point16->x); /* x (2 bytes) */
	Stream_Write_UINT16(s, point16->y); /* y (2 bytes) */
	return CHANNEL_RC_OK;
}