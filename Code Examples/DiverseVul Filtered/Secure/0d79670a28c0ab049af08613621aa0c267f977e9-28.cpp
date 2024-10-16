static HRESULT STDMETHODCALLTYPE CliprdrDataObject_GetData(IDataObject* This, FORMATETC* pFormatEtc,
                                                           STGMEDIUM* pMedium)
{
	ULONG i;
	LONG idx;
	CliprdrDataObject* instance = (CliprdrDataObject*)This;
	wfClipboard* clipboard;

	if (!pFormatEtc || !pMedium || !instance)
		return E_INVALIDARG;

	clipboard = (wfClipboard*)instance->m_pData;

	if (!clipboard)
		return E_INVALIDARG;

	if ((idx = cliprdr_lookup_format(instance, pFormatEtc)) == -1)
		return DV_E_FORMATETC;

	pMedium->tymed = instance->m_pFormatEtc[idx].tymed;
	pMedium->pUnkForRelease = 0;

	if (instance->m_pFormatEtc[idx].cfFormat == RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW))
	{
		FILEGROUPDESCRIPTOR* dsc;
		DWORD remote = get_remote_format_id(clipboard, instance->m_pFormatEtc[idx].cfFormat);

		if (cliprdr_send_data_request(clipboard, remote) != 0)
			return E_UNEXPECTED;

		pMedium->hGlobal = clipboard->hmem; /* points to a FILEGROUPDESCRIPTOR structure */
		/* GlobalLock returns a pointer to the first byte of the memory block,
		 * in which is a FILEGROUPDESCRIPTOR structure, whose first UINT member
		 * is the number of FILEDESCRIPTOR's */
		dsc = (FILEGROUPDESCRIPTOR*)GlobalLock(clipboard->hmem);
		instance->m_nStreams = dsc->cItems;
		GlobalUnlock(clipboard->hmem);

		if (instance->m_nStreams > 0)
		{
			if (!instance->m_pStream)
			{
				instance->m_pStream = (LPSTREAM*)calloc(instance->m_nStreams, sizeof(LPSTREAM));

				if (instance->m_pStream)
				{
					for (i = 0; i < instance->m_nStreams; i++)
					{
						instance->m_pStream[i] =
						    (IStream*)CliprdrStream_New(i, clipboard, &dsc->fgd[i]);

						if (!instance->m_pStream[i])
							return E_OUTOFMEMORY;
					}
				}
			}
		}

		if (!instance->m_pStream)
		{
			if (clipboard->hmem)
			{
				GlobalFree(clipboard->hmem);
				clipboard->hmem = NULL;
			}

			pMedium->hGlobal = NULL;
			return E_OUTOFMEMORY;
		}
	}
	else if (instance->m_pFormatEtc[idx].cfFormat == RegisterClipboardFormat(CFSTR_FILECONTENTS))
	{
		if ((pFormatEtc->lindex >= 0) && ((ULONG)pFormatEtc->lindex < instance->m_nStreams))
		{
			pMedium->pstm = instance->m_pStream[pFormatEtc->lindex];
			IDataObject_AddRef(instance->m_pStream[pFormatEtc->lindex]);
		}
		else
			return E_INVALIDARG;
	}
	else
		return E_UNEXPECTED;

	return S_OK;
}