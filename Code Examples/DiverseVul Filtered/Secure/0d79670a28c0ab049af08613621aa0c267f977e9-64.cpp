static UINT32 get_local_format_id_by_name(wfClipboard* clipboard, const TCHAR* format_name)
{
	size_t i;
	formatMapping* map;
	WCHAR* unicode_name;
#if !defined(UNICODE)
	size_t size;
#endif

	if (!clipboard || !format_name)
		return 0;

#if defined(UNICODE)
	unicode_name = _wcsdup(format_name);
#else
	size = _tcslen(format_name);
	unicode_name = calloc(size + 1, sizeof(WCHAR));

	if (!unicode_name)
		return 0;

	MultiByteToWideChar(CP_OEMCP, 0, format_name, strlen(format_name), unicode_name, size);
#endif

	if (!unicode_name)
		return 0;

	for (i = 0; i < clipboard->map_size; i++)
	{
		map = &clipboard->format_mappings[i];

		if (map->name)
		{
			if (wcscmp(map->name, unicode_name) == 0)
			{
				free(unicode_name);
				return map->local_format_id;
			}
		}
	}

	free(unicode_name);
	return 0;
}