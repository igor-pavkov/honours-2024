static ULONG STDMETHODCALLTYPE CliprdrEnumFORMATETC_AddRef(IEnumFORMATETC* This)
{
	CliprdrEnumFORMATETC* instance = (CliprdrEnumFORMATETC*)This;

	if (!instance)
		return 0;

	return InterlockedIncrement(&instance->m_lRefCount);
}