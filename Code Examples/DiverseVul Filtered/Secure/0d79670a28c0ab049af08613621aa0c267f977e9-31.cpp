static HRESULT STDMETHODCALLTYPE CliprdrStream_Stat(IStream* This, STATSTG* pstatstg,
                                                    DWORD grfStatFlag)
{
	CliprdrStream* instance = (CliprdrStream*)This;

	if (!instance)
		return E_INVALIDARG;

	if (pstatstg == NULL)
		return STG_E_INVALIDPOINTER;

	ZeroMemory(pstatstg, sizeof(STATSTG));

	switch (grfStatFlag)
	{
		case STATFLAG_DEFAULT:
			return STG_E_INSUFFICIENTMEMORY;

		case STATFLAG_NONAME:
			pstatstg->cbSize.QuadPart = instance->m_lSize.QuadPart;
			pstatstg->grfLocksSupported = LOCK_EXCLUSIVE;
			pstatstg->grfMode = GENERIC_READ;
			pstatstg->grfStateBits = 0;
			pstatstg->type = STGTY_STREAM;
			break;

		case STATFLAG_NOOPEN:
			return STG_E_INVALIDFLAG;

		default:
			return STG_E_INVALIDFLAG;
	}

	return S_OK;
}