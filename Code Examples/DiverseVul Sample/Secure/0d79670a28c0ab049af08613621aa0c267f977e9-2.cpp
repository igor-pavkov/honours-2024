static HRESULT STDMETHODCALLTYPE CliprdrEnumFORMATETC_Next(IEnumFORMATETC* This, ULONG celt,
                                                           FORMATETC* rgelt, ULONG* pceltFetched)
{
	ULONG copied = 0;
	CliprdrEnumFORMATETC* instance = (CliprdrEnumFORMATETC*)This;

	if (!instance || !celt || !rgelt)
		return E_INVALIDARG;

	while ((instance->m_nIndex < instance->m_nNumFormats) && (copied < celt))
	{
		cliprdr_format_deep_copy(&rgelt[copied++], &instance->m_pFormatEtc[instance->m_nIndex++]);
	}

	if (pceltFetched != 0)
		*pceltFetched = copied;

	return (copied == celt) ? S_OK : E_FAIL;
}