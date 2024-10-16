static UINT cliprdr_send_data_request(wfClipboard* clipboard, UINT32 formatId)
{
	UINT rc;
	UINT32 remoteFormatId;
	CLIPRDR_FORMAT_DATA_REQUEST formatDataRequest;

	if (!clipboard || !clipboard->context || !clipboard->context->ClientFormatDataRequest)
		return ERROR_INTERNAL_ERROR;

	remoteFormatId = get_remote_format_id(clipboard, formatId);

	formatDataRequest.requestedFormatId = remoteFormatId;
	clipboard->requestedFormatId = formatId;
	rc = clipboard->context->ClientFormatDataRequest(clipboard->context, &formatDataRequest);

	if (WaitForSingleObject(clipboard->response_data_event, INFINITE) != WAIT_OBJECT_0)
		rc = ERROR_INTERNAL_ERROR;
	else if (!ResetEvent(clipboard->response_data_event))
		rc = ERROR_INTERNAL_ERROR;

	return rc;
}