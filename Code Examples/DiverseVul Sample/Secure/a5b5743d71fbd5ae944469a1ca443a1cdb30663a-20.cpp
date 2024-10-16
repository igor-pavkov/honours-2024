PHP_FUNCTION(filter_list)
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	for (i = 0; i < size; ++i) {
		add_next_index_string(return_value, (char *)filter_list[i].name, 1);
	}
}