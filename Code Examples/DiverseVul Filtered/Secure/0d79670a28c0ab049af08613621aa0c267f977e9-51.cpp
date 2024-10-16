static HRESULT STDMETHODCALLTYPE CliprdrStream_CopyTo(IStream* This, IStream* pstm,
                                                      ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead,
                                                      ULARGE_INTEGER* pcbWritten)
{
	(void)This;
	(void)pstm;
	(void)cb;
	(void)pcbRead;
	(void)pcbWritten;
	return E_NOTIMPL;
}