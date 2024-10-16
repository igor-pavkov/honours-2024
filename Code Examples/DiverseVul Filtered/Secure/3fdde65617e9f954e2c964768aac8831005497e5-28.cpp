ZEND_INI_MH(OnUpdateDecode)
{
	if (new_value) {
		const zend_encoding **return_list;
		size_t return_size;
		if (FAILURE == zend_multibyte_parse_encoding_list(ZSTR_VAL(new_value), ZSTR_LEN(new_value),
	&return_list, &return_size, 0)) {
			php_error_docref(NULL, E_WARNING, "Illegal encoding ignored: '%s'", ZSTR_VAL(new_value));
			return FAILURE;
		}
		pefree((void *) return_list, 0);
	}
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}