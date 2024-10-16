static HRESULT STDMETHODCALLTYPE CliprdrDataObject_DUnadvise(IDataObject* This, DWORD dwConnection)
{
	(void)This;
	(void)dwConnection;
	return OLE_E_ADVISENOTSUPPORTED;
}