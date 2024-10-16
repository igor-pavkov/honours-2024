BOOL update_read_suppress_output(rdpUpdate* update, wStream* s)
{
	RECTANGLE_16* prect = NULL;
	RECTANGLE_16 rect = { 0 };
	BYTE allowDisplayUpdates;

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT8(s, allowDisplayUpdates);
	Stream_Seek(s, 3); /* pad3Octects */

	if (allowDisplayUpdates > 0)
	{
		if (Stream_GetRemainingLength(s) < sizeof(RECTANGLE_16))
			return FALSE;
		Stream_Read_UINT16(s, rect.left);
		Stream_Read_UINT16(s, rect.top);
		Stream_Read_UINT16(s, rect.right);
		Stream_Read_UINT16(s, rect.bottom);

		prect = &rect;
	}

	if (update->context->settings->SuppressOutput)
		IFCALL(update->SuppressOutput, update->context, allowDisplayUpdates, prect);
	else
		WLog_Print(update->log, WLOG_WARN, "ignoring suppress output request from client");

	return TRUE;
}