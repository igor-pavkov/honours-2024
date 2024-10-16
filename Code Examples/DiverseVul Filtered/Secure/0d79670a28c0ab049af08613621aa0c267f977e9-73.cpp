static ULONG STDMETHODCALLTYPE CliprdrDataObject_AddRef(IDataObject* This)
{
	CliprdrDataObject* instance = (CliprdrDataObject*)This;

	if (!instance)
		return E_INVALIDARG;

	return InterlockedIncrement(&instance->m_lRefCount);
}