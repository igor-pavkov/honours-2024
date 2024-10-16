static CliprdrDataObject* CliprdrDataObject_New(FORMATETC* fmtetc, STGMEDIUM* stgmed, ULONG count,
                                                void* data)
{
	CliprdrDataObject* instance;
	IDataObject* iDataObject;
	instance = (CliprdrDataObject*)calloc(1, sizeof(CliprdrDataObject));

	if (!instance)
		goto error;

	iDataObject = &instance->iDataObject;
	iDataObject->lpVtbl = (IDataObjectVtbl*)calloc(1, sizeof(IDataObjectVtbl));

	if (!iDataObject->lpVtbl)
		goto error;

	iDataObject->lpVtbl->QueryInterface = CliprdrDataObject_QueryInterface;
	iDataObject->lpVtbl->AddRef = CliprdrDataObject_AddRef;
	iDataObject->lpVtbl->Release = CliprdrDataObject_Release;
	iDataObject->lpVtbl->GetData = CliprdrDataObject_GetData;
	iDataObject->lpVtbl->GetDataHere = CliprdrDataObject_GetDataHere;
	iDataObject->lpVtbl->QueryGetData = CliprdrDataObject_QueryGetData;
	iDataObject->lpVtbl->GetCanonicalFormatEtc = CliprdrDataObject_GetCanonicalFormatEtc;
	iDataObject->lpVtbl->SetData = CliprdrDataObject_SetData;
	iDataObject->lpVtbl->EnumFormatEtc = CliprdrDataObject_EnumFormatEtc;
	iDataObject->lpVtbl->DAdvise = CliprdrDataObject_DAdvise;
	iDataObject->lpVtbl->DUnadvise = CliprdrDataObject_DUnadvise;
	iDataObject->lpVtbl->EnumDAdvise = CliprdrDataObject_EnumDAdvise;
	instance->m_lRefCount = 1;
	instance->m_nNumFormats = count;
	instance->m_pData = data;
	instance->m_nStreams = 0;
	instance->m_pStream = NULL;

	if (count > 0)
	{
		ULONG i;
		instance->m_pFormatEtc = (FORMATETC*)calloc(count, sizeof(FORMATETC));

		if (!instance->m_pFormatEtc)
			goto error;

		instance->m_pStgMedium = (STGMEDIUM*)calloc(count, sizeof(STGMEDIUM));

		if (!instance->m_pStgMedium)
			goto error;

		for (i = 0; i < count; i++)
		{
			instance->m_pFormatEtc[i] = fmtetc[i];
			instance->m_pStgMedium[i] = stgmed[i];
		}
	}

	return instance;
error:
	CliprdrDataObject_Delete(instance);
	return NULL;
}