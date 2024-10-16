static HRESULT STDMETHODCALLTYPE CliprdrStream_QueryInterface(IStream* This, REFIID riid,
                                                              void** ppvObject)
{
	if (IsEqualIID(riid, &IID_IStream) || IsEqualIID(riid, &IID_IUnknown))
	{
		IStream_AddRef(This);
		*ppvObject = This;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}