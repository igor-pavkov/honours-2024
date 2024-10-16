BOOL wf_cliprdr_uninit(wfContext* wfc, CliprdrClientContext* cliprdr)
{
	wfClipboard* clipboard;

	if (!wfc || !cliprdr)
		return FALSE;

	clipboard = wfc->clipboard;

	if (!clipboard)
		return FALSE;

	cliprdr->custom = NULL;

	if (clipboard->hwnd)
		PostMessage(clipboard->hwnd, WM_QUIT, 0, 0);

	if (clipboard->thread)
	{
		WaitForSingleObject(clipboard->thread, INFINITE);
		CloseHandle(clipboard->thread);
	}

	if (clipboard->response_data_event)
		CloseHandle(clipboard->response_data_event);

	if (clipboard->req_fevent)
		CloseHandle(clipboard->req_fevent);

	clear_file_array(clipboard);
	clear_format_map(clipboard);
	free(clipboard->format_mappings);
	free(clipboard);
	return TRUE;
}