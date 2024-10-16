BOOL update_read_refresh_rect(rdpUpdate* update, wStream* s)
{
	int index;
	BYTE numberOfAreas;
	RECTANGLE_16* areas;

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT8(s, numberOfAreas);
	Stream_Seek(s, 3); /* pad3Octects */

	if (Stream_GetRemainingLength(s) < ((size_t)numberOfAreas * 4 * 2))
		return FALSE;

	areas = (RECTANGLE_16*)calloc(numberOfAreas, sizeof(RECTANGLE_16));

	if (!areas)
		return FALSE;

	for (index = 0; index < numberOfAreas; index++)
	{
		Stream_Read_UINT16(s, areas[index].left);
		Stream_Read_UINT16(s, areas[index].top);
		Stream_Read_UINT16(s, areas[index].right);
		Stream_Read_UINT16(s, areas[index].bottom);
	}

	if (update->context->settings->RefreshRect)
		IFCALL(update->RefreshRect, update->context, numberOfAreas, areas);
	else
		WLog_Print(update->log, WLOG_WARN, "ignoring refresh rect request from client");

	free(areas);
	return TRUE;
}