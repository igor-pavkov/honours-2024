static filter_list_entry php_find_filter(long id) /* {{{ */
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	for (i = 0; i < size; ++i) {
		if (filter_list[i].id == id) {
			return filter_list[i];
		}
	}
	/* Fallback to "string" filter */
	for (i = 0; i < size; ++i) {
		if (filter_list[i].id == FILTER_DEFAULT) {
			return filter_list[i];
		}
	}
	/* To shut up GCC */
	return filter_list[0];
}