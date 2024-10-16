static HRESULT STDMETHODCALLTYPE CliprdrEnumFORMATETC_QueryInterface(IEnumFORMATETC* This,
                                                                     REFIID riid, void** ppvObject)
{
	(void)This;

	if (IsEqualIID(riid, &IID_IEnumFORMATETC) || IsEqualIID(riid, &IID_IUnknown))
	{
		IEnumFORMATETC_AddRef(This);
		*ppvObject = This;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}