static INLINE BOOL update_write_coord(wStream* s, INT32 coord)
{
	Stream_Write_UINT16(s, coord);
	return TRUE;
}