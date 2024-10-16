static HRESULT STDMETHODCALLTYPE CliprdrStream_Seek(IStream* This, LARGE_INTEGER dlibMove,
                                                    DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	ULONGLONG newoffset;
	CliprdrStream* instance = (CliprdrStream*)This;

	if (!instance)
		return E_INVALIDARG;

	newoffset = instance->m_lOffset.QuadPart;

	switch (dwOrigin)
	{
		case STREAM_SEEK_SET:
			newoffset = dlibMove.QuadPart;
			break;

		case STREAM_SEEK_CUR:
			newoffset += dlibMove.QuadPart;
			break;

		case STREAM_SEEK_END:
			newoffset = instance->m_lSize.QuadPart + dlibMove.QuadPart;
			break;

		default:
			return E_INVALIDARG;
	}

	if (newoffset < 0 || newoffset >= instance->m_lSize.QuadPart)
		return E_FAIL;

	instance->m_lOffset.QuadPart = newoffset;

	if (plibNewPosition)
		plibNewPosition->QuadPart = instance->m_lOffset.QuadPart;

	return S_OK;
}