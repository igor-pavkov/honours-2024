static LONG cliprdr_lookup_format(CliprdrDataObject* instance, FORMATETC* pFormatEtc)
{
	ULONG i;

	if (!instance || !pFormatEtc)
		return -1;

	for (i = 0; i < instance->m_nNumFormats; i++)
	{
		if ((pFormatEtc->tymed & instance->m_pFormatEtc[i].tymed) &&
		    pFormatEtc->cfFormat == instance->m_pFormatEtc[i].cfFormat &&
		    pFormatEtc->dwAspect & instance->m_pFormatEtc[i].dwAspect)
		{
			return (LONG)i;
		}
	}

	return -1;
}