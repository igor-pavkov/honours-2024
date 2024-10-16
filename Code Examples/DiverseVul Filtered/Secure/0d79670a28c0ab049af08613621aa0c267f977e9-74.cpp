static HRESULT STDMETHODCALLTYPE CliprdrDataObject_EnumFormatEtc(IDataObject* This,
                                                                 DWORD dwDirection,
                                                                 IEnumFORMATETC** ppenumFormatEtc)
{
	CliprdrDataObject* instance = (CliprdrDataObject*)This;

	if (!instance || !ppenumFormatEtc)
		return E_INVALIDARG;

	if (dwDirection == DATADIR_GET)
	{
		*ppenumFormatEtc = (IEnumFORMATETC*)CliprdrEnumFORMATETC_New(instance->m_nNumFormats,
		                                                             instance->m_pFormatEtc);
		return (*ppenumFormatEtc) ? S_OK : E_OUTOFMEMORY;
	}
	else
	{
		return E_NOTIMPL;
	}
}