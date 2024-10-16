static void cliprdr_format_deep_copy(FORMATETC* dest, FORMATETC* source)
{
	*dest = *source;

	if (source->ptd)
	{
		dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

		if (dest->ptd)
			*(dest->ptd) = *(source->ptd);
	}
}