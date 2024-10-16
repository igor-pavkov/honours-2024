static HRESULT STDMETHODCALLTYPE CliprdrEnumFORMATETC_Clone(IEnumFORMATETC* This,
                                                            IEnumFORMATETC** ppEnum)
{
	CliprdrEnumFORMATETC* instance = (CliprdrEnumFORMATETC*)This;

	if (!instance || !ppEnum)
		return E_INVALIDARG;

	*ppEnum =
	    (IEnumFORMATETC*)CliprdrEnumFORMATETC_New(instance->m_nNumFormats, instance->m_pFormatEtc);

	if (!*ppEnum)
		return E_OUTOFMEMORY;

	((CliprdrEnumFORMATETC*)*ppEnum)->m_nIndex = instance->m_nIndex;
	return S_OK;
}