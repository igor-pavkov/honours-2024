static void filter_map_update(filter_map *map, int flag, const unsigned char *allowed_list)
{
	int l, i;

	l = strlen((const char*)allowed_list);
	for (i = 0; i < l; ++i) {
		(*map)[allowed_list[i]] = flag;
	}
}