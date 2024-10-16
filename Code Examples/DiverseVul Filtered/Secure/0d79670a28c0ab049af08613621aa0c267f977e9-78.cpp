static HRESULT STDMETHODCALLTYPE CliprdrDataObject_EnumDAdvise(IDataObject* This,
                                                               IEnumSTATDATA** ppenumAdvise)
{
	(void)This;
	(void)ppenumAdvise;
	return OLE_E_ADVISENOTSUPPORTED;
}