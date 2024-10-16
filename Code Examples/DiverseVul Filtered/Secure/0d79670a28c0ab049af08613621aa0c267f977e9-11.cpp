static HRESULT STDMETHODCALLTYPE CliprdrStream_Read(IStream* This, void* pv, ULONG cb,
                                                    ULONG* pcbRead)
{
	int ret;
	CliprdrStream* instance = (CliprdrStream*)This;
	wfClipboard* clipboard;

	if (!pv || !pcbRead || !instance)
		return E_INVALIDARG;

	clipboard = (wfClipboard*)instance->m_pData;
	*pcbRead = 0;

	if (instance->m_lOffset.QuadPart >= instance->m_lSize.QuadPart)
		return S_FALSE;

	ret = cliprdr_send_request_filecontents(clipboard, (void*)This, instance->m_lIndex,
	                                        FILECONTENTS_RANGE, instance->m_lOffset.HighPart,
	                                        instance->m_lOffset.LowPart, cb);

	if (ret < 0)
		return E_FAIL;

	if (clipboard->req_fdata)
	{
		CopyMemory(pv, clipboard->req_fdata, clipboard->req_fsize);
		free(clipboard->req_fdata);
	}

	*pcbRead = clipboard->req_fsize;
	instance->m_lOffset.QuadPart += clipboard->req_fsize;

	if (clipboard->req_fsize < cb)
		return S_FALSE;

	return S_OK;
}