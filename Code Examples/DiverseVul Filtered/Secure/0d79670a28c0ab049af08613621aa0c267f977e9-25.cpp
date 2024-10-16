static ULONG STDMETHODCALLTYPE CliprdrDataObject_Release(IDataObject* This)
{
	LONG count;
	CliprdrDataObject* instance = (CliprdrDataObject*)This;

	if (!instance)
		return E_INVALIDARG;

	count = InterlockedDecrement(&instance->m_lRefCount);

	if (count == 0)
	{
		CliprdrDataObject_Delete(instance);
		return 0;
	}
	else
		return count;
}