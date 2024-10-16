BOOL wf_cliprdr_init(wfContext* wfc, CliprdrClientContext* cliprdr)
{
	wfClipboard* clipboard;
	rdpContext* context = (rdpContext*)wfc;

	if (!context || !cliprdr)
		return FALSE;

	wfc->clipboard = (wfClipboard*)calloc(1, sizeof(wfClipboard));

	if (!wfc->clipboard)
		return FALSE;

	clipboard = wfc->clipboard;
	clipboard->wfc = wfc;
	clipboard->context = cliprdr;
	clipboard->channels = context->channels;
	clipboard->sync = FALSE;
	clipboard->map_capacity = 32;
	clipboard->map_size = 0;
	clipboard->hUser32 = LoadLibraryA("user32.dll");

	if (clipboard->hUser32)
	{
		clipboard->AddClipboardFormatListener = (fnAddClipboardFormatListener)GetProcAddress(
		    clipboard->hUser32, "AddClipboardFormatListener");
		clipboard->RemoveClipboardFormatListener = (fnRemoveClipboardFormatListener)GetProcAddress(
		    clipboard->hUser32, "RemoveClipboardFormatListener");
		clipboard->GetUpdatedClipboardFormats = (fnGetUpdatedClipboardFormats)GetProcAddress(
		    clipboard->hUser32, "GetUpdatedClipboardFormats");
	}

	if (!(clipboard->hUser32 && clipboard->AddClipboardFormatListener &&
	      clipboard->RemoveClipboardFormatListener && clipboard->GetUpdatedClipboardFormats))
		clipboard->legacyApi = TRUE;

	if (!(clipboard->format_mappings =
	          (formatMapping*)calloc(clipboard->map_capacity, sizeof(formatMapping))))
		goto error;

	if (!(clipboard->response_data_event = CreateEvent(NULL, TRUE, FALSE, NULL)))
		goto error;

	if (!(clipboard->req_fevent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		goto error;

	if (!(clipboard->thread = CreateThread(NULL, 0, cliprdr_thread_func, clipboard, 0, NULL)))
		goto error;

	cliprdr->MonitorReady = wf_cliprdr_monitor_ready;
	cliprdr->ServerCapabilities = wf_cliprdr_server_capabilities;
	cliprdr->ServerFormatList = wf_cliprdr_server_format_list;
	cliprdr->ServerFormatListResponse = wf_cliprdr_server_format_list_response;
	cliprdr->ServerLockClipboardData = wf_cliprdr_server_lock_clipboard_data;
	cliprdr->ServerUnlockClipboardData = wf_cliprdr_server_unlock_clipboard_data;
	cliprdr->ServerFormatDataRequest = wf_cliprdr_server_format_data_request;
	cliprdr->ServerFormatDataResponse = wf_cliprdr_server_format_data_response;
	cliprdr->ServerFileContentsRequest = wf_cliprdr_server_file_contents_request;
	cliprdr->ServerFileContentsResponse = wf_cliprdr_server_file_contents_response;
	cliprdr->custom = (void*)wfc->clipboard;
	return TRUE;
error:
	wf_cliprdr_uninit(wfc, cliprdr);
	return FALSE;
}