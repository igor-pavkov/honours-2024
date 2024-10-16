static HRESULT STDMETHODCALLTYPE CliprdrDataObject_GetCanonicalFormatEtc(IDataObject* This,
                                                                         FORMATETC* pformatectIn,
                                                                         FORMATETC* pformatetcOut)
{
	(void)This;
	(void)pformatectIn;

	if (!pformatetcOut)
		return E_INVALIDARG;

	pformatetcOut->ptd = NULL;
	return E_NOTIMPL;
}