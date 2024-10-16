static UINT cliprdr_send_response_filecontents(wfClipboard* clipboard, UINT32 streamId, UINT32 size,
                                               BYTE* data)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE fileContentsResponse;

	if (!clipboard || !clipboard->context || !clipboard->context->ClientFileContentsResponse)
		return ERROR_INTERNAL_ERROR;

	fileContentsResponse.streamId = streamId;
	fileContentsResponse.cbRequested = size;
	fileContentsResponse.requestedData = data;
	fileContentsResponse.msgFlags = CB_RESPONSE_OK;
	return clipboard->context->ClientFileContentsResponse(clipboard->context,
	                                                      &fileContentsResponse);
}