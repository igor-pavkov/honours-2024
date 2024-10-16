static HRESULT STDMETHODCALLTYPE CliprdrDataObject_QueryInterface(IDataObject* This, REFIID riid,
                                                                  void** ppvObject)
{
	(void)This;

	if (!ppvObject)
		return E_INVALIDARG;

	if (IsEqualIID(riid, &IID_IDataObject) || IsEqualIID(riid, &IID_IUnknown))
	{
		IDataObject_AddRef(This);
		*ppvObject = This;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}