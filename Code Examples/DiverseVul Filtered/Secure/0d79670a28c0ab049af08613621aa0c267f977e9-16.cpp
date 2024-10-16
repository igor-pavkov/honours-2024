static BOOL clear_format_map(wfClipboard* clipboard)
{
	size_t i;
	formatMapping* map;

	if (!clipboard)
		return FALSE;

	if (clipboard->format_mappings)
	{
		for (i = 0; i < clipboard->map_capacity; i++)
		{
			map = &clipboard->format_mappings[i];
			map->remote_format_id = 0;
			map->local_format_id = 0;
			free(map->name);
			map->name = NULL;
		}
	}

	clipboard->map_size = 0;
	return TRUE;
}