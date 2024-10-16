PHP_FUNCTION(ldap_get_values_len)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attr;
	struct berval **ldap_value_len;
	int i, num_values, attr_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrs", &link, &result_entry, &attr, &attr_len) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((ldap_value_len = ldap_get_values_len(ld->link, resultentry->data, attr)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot get the value(s) of attribute %s", ldap_err2string(_get_lderrno(ld->link)));
		RETURN_FALSE;
	}

	num_values = ldap_count_values_len(ldap_value_len);
	array_init(return_value);

	for (i=0; i<num_values; i++) {
		add_next_index_stringl(return_value, ldap_value_len[i]->bv_val, ldap_value_len[i]->bv_len, 1);
	}

	add_assoc_long(return_value, "count", num_values);
	ldap_value_free_len(ldap_value_len);

}