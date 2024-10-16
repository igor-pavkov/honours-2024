static ULONG STDMETHODCALLTYPE CliprdrStream_Release(IStream* This)
{
	LONG count;
	CliprdrStream* instance = (CliprdrStream*)This;

	if (!instance)
		return 0;

	count = InterlockedDecrement(&instance->m_lRefCount);

	if (count == 0)
	{
		CliprdrStream_Delete(instance);
		return 0;
	}
	else
	{
		return count;
	}
}