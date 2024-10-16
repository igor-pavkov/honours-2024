void update_free_window_icon_info(ICON_INFO* iconInfo)
{
	if (!iconInfo)
		return;

	free(iconInfo->bitsColor);
	iconInfo->bitsColor = NULL;
	free(iconInfo->bitsMask);
	iconInfo->bitsMask = NULL;
	free(iconInfo->colorTable);
	iconInfo->colorTable = NULL;
}