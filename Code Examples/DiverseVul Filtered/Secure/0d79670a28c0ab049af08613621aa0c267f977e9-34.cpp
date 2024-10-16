static ULONG STDMETHODCALLTYPE CliprdrStream_AddRef(IStream* This)
{
	CliprdrStream* instance = (CliprdrStream*)This;

	if (!instance)
		return 0;

	return InterlockedIncrement(&instance->m_lRefCount);
}