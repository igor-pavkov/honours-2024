static HRESULT STDMETHODCALLTYPE CliprdrStream_Write(IStream* This, const void* pv, ULONG cb,
                                                     ULONG* pcbWritten)
{
	(void)This;
	(void)pv;
	(void)cb;
	(void)pcbWritten;
	return STG_E_ACCESSDENIED;
}