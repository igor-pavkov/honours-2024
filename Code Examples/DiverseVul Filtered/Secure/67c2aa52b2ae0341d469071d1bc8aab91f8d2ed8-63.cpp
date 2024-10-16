static BOOL update_read_frame_marker_order(wStream* s, FRAME_MARKER_ORDER* frame_marker)
{
	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT32(s, frame_marker->action); /* action (4 bytes) */
	return TRUE;
}