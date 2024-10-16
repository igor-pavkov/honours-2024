UINT cliprdr_send_request_filecontents(wfClipboard* clipboard, const void* streamid, ULONG index,
                                       UINT32 flag, DWORD positionhigh, DWORD positionlow,
                                       ULONG nreq)
{
	UINT rc;
	CLIPRDR_FILE_CONTENTS_REQUEST fileContentsRequest;

	if (!clipboard || !clipboard->context || !clipboard->context->ClientFileContentsRequest)
		return ERROR_INTERNAL_ERROR;

	fileContentsRequest.streamId = (UINT32)(ULONG_PTR)streamid;
	fileContentsRequest.listIndex = index;
	fileContentsRequest.dwFlags = flag;
	fileContentsRequest.nPositionLow = positionlow;
	fileContentsRequest.nPositionHigh = positionhigh;
	fileContentsRequest.cbRequested = nreq;
	fileContentsRequest.clipDataId = 0;
	fileContentsRequest.msgFlags = 0;
	rc = clipboard->context->ClientFileContentsRequest(clipboard->context, &fileContentsRequest);

	if (WaitForSingleObject(clipboard->req_fevent, INFINITE) != WAIT_OBJECT_0)
		rc = ERROR_INTERNAL_ERROR;
	else if (!ResetEvent(clipboard->req_fevent))
		rc = ERROR_INTERNAL_ERROR;

	return rc;
}