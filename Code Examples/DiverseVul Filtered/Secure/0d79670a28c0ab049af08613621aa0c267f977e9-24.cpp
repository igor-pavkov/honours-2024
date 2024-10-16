static void map_ensure_capacity(wfClipboard* clipboard)
{
	if (!clipboard)
		return;

	if (clipboard->map_size >= clipboard->map_capacity)
	{
		size_t new_size;
		formatMapping* new_map;
		new_size = clipboard->map_capacity * 2;
		new_map =
		    (formatMapping*)realloc(clipboard->format_mappings, sizeof(formatMapping) * new_size);

		if (!new_map)
			return;

		clipboard->format_mappings = new_map;
		clipboard->map_capacity = new_size;
	}
}