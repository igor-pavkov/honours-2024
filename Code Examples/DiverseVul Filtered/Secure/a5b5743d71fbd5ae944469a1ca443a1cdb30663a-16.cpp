PHP_FUNCTION(filter_id)
{
	int i, filter_len;
	int size = sizeof(filter_list) / sizeof(filter_list_entry);
	char *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filter, &filter_len) == FAILURE) {
		return;
	}

	for (i = 0; i < size; ++i) {
		if (strcmp(filter_list[i].name, filter) == 0) {
			RETURN_LONG(filter_list[i].id);
		}
	}

	RETURN_FALSE;
}