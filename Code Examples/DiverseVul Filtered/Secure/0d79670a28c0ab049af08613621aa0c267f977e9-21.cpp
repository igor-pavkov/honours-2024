CliprdrEnumFORMATETC* CliprdrEnumFORMATETC_New(ULONG nFormats, FORMATETC* pFormatEtc)
{
	ULONG i;
	CliprdrEnumFORMATETC* instance;
	IEnumFORMATETC* iEnumFORMATETC;

	if ((nFormats != 0) && !pFormatEtc)
		return NULL;

	instance = (CliprdrEnumFORMATETC*)calloc(1, sizeof(CliprdrEnumFORMATETC));

	if (!instance)
		goto error;

	iEnumFORMATETC = &instance->iEnumFORMATETC;
	iEnumFORMATETC->lpVtbl = (IEnumFORMATETCVtbl*)calloc(1, sizeof(IEnumFORMATETCVtbl));

	if (!iEnumFORMATETC->lpVtbl)
		goto error;

	iEnumFORMATETC->lpVtbl->QueryInterface = CliprdrEnumFORMATETC_QueryInterface;
	iEnumFORMATETC->lpVtbl->AddRef = CliprdrEnumFORMATETC_AddRef;
	iEnumFORMATETC->lpVtbl->Release = CliprdrEnumFORMATETC_Release;
	iEnumFORMATETC->lpVtbl->Next = CliprdrEnumFORMATETC_Next;
	iEnumFORMATETC->lpVtbl->Skip = CliprdrEnumFORMATETC_Skip;
	iEnumFORMATETC->lpVtbl->Reset = CliprdrEnumFORMATETC_Reset;
	iEnumFORMATETC->lpVtbl->Clone = CliprdrEnumFORMATETC_Clone;
	instance->m_lRefCount = 1;
	instance->m_nIndex = 0;
	instance->m_nNumFormats = nFormats;

	if (nFormats > 0)
	{
		instance->m_pFormatEtc = (FORMATETC*)calloc(nFormats, sizeof(FORMATETC));

		if (!instance->m_pFormatEtc)
			goto error;

		for (i = 0; i < nFormats; i++)
			cliprdr_format_deep_copy(&instance->m_pFormatEtc[i], &pFormatEtc[i]);
	}

	return instance;
error:
	CliprdrEnumFORMATETC_Delete(instance);
	return NULL;
}