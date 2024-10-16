static PHP_INI_MH(UpdateDefaultFilter) /* {{{ */
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	for (i = 0; i < size; ++i) {
		if ((strcasecmp(new_value, filter_list[i].name) == 0)) {
			IF_G(default_filter) = filter_list[i].id;
			return SUCCESS;
		}
	}
	/* Fallback to the default filter */
	IF_G(default_filter) = FILTER_DEFAULT;
	return SUCCESS;
}