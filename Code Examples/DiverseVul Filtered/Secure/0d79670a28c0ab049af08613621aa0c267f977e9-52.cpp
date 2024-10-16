static HRESULT STDMETHODCALLTYPE CliprdrEnumFORMATETC_Skip(IEnumFORMATETC* This, ULONG celt)
{
	CliprdrEnumFORMATETC* instance = (CliprdrEnumFORMATETC*)This;

	if (!instance)
		return E_INVALIDARG;

	if (instance->m_nIndex + (LONG)celt > instance->m_nNumFormats)
		return E_FAIL;

	instance->m_nIndex += celt;
	return S_OK;
}