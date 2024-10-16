static BOOL update_read_switch_surface_order(wStream* s, SWITCH_SURFACE_ORDER* switch_surface)
{
	if (Stream_GetRemainingLength(s) < 2)
		return FALSE;

	Stream_Read_UINT16(s, switch_surface->bitmapId); /* bitmapId (2 bytes) */
	return TRUE;
}