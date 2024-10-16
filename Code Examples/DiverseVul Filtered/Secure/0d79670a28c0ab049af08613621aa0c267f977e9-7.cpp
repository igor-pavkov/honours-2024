static UINT32 get_remote_format_id(wfClipboard* clipboard, UINT32 local_format)
{
	UINT32 i;
	formatMapping* map;

	if (!clipboard)
		return 0;

	for (i = 0; i < clipboard->map_size; i++)
	{
		map = &clipboard->format_mappings[i];

		if (map->local_format_id == local_format)
			return map->remote_format_id;
	}

	return local_format;
}