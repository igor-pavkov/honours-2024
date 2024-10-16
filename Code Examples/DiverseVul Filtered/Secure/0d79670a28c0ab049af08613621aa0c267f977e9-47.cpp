static HRESULT STDMETHODCALLTYPE CliprdrDataObject_QueryGetData(IDataObject* This,
                                                                FORMATETC* pformatetc)
{
	CliprdrDataObject* instance = (CliprdrDataObject*)This;

	if (!pformatetc)
		return E_INVALIDARG;

	if (cliprdr_lookup_format(instance, pformatetc) == -1)
		return DV_E_FORMATETC;

	return S_OK;
}