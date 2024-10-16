static HRESULT STDMETHODCALLTYPE CliprdrStream_UnlockRegion(IStream* This, ULARGE_INTEGER libOffset,
                                                            ULARGE_INTEGER cb, DWORD dwLockType)
{
	(void)This;
	(void)libOffset;
	(void)cb;
	(void)dwLockType;
	return E_NOTIMPL;
}