BOOL update_write_switch_surface_order(wStream* s, const SWITCH_SURFACE_ORDER* switch_surface)
{
	int inf = update_approximate_switch_surface_order(switch_surface);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	Stream_Write_UINT16(s, switch_surface->bitmapId); /* bitmapId (2 bytes) */
	return TRUE;
}