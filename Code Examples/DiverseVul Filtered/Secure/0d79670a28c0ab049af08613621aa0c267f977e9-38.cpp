static HRESULT STDMETHODCALLTYPE CliprdrDataObject_DAdvise(IDataObject* This, FORMATETC* pformatetc,
                                                           DWORD advf, IAdviseSink* pAdvSink,
                                                           DWORD* pdwConnection)
{
	(void)This;
	(void)pformatetc;
	(void)advf;
	(void)pAdvSink;
	(void)pdwConnection;
	return OLE_E_ADVISENOTSUPPORTED;
}