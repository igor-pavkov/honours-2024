static UINT wf_cliprdr_monitor_ready(CliprdrClientContext* context,
                                     const CLIPRDR_MONITOR_READY* monitorReady)
{
	UINT rc;
	wfClipboard* clipboard = (wfClipboard*)context->custom;

	if (!context || !monitorReady)
		return ERROR_INTERNAL_ERROR;

	clipboard->sync = TRUE;
	rc = wf_cliprdr_send_client_capabilities(clipboard);

	if (rc != CHANNEL_RC_OK)
		return rc;

	return cliprdr_send_format_list(clipboard);
}