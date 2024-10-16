static HRESULT STDMETHODCALLTYPE CliprdrEnumFORMATETC_Reset(IEnumFORMATETC* This)
{
	CliprdrEnumFORMATETC* instance = (CliprdrEnumFORMATETC*)This;

	if (!instance)
		return E_INVALIDARG;

	instance->m_nIndex = 0;
	return S_OK;
}