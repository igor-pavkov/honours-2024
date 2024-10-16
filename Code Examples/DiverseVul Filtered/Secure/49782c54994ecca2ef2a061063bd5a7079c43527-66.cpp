PHP_FUNCTION(ldap_explode_dn)
{
	long with_attrib;
	char *dn, **ldap_value;
	int i, count, dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &dn, &dn_len, &with_attrib) != SUCCESS) {
		return;
	}

	if (!(ldap_value = ldap_explode_dn(dn, with_attrib))) {
		/* Invalid parameters were passed to ldap_explode_dn */
		RETURN_FALSE;
	}

	i=0;
	while (ldap_value[i] != NULL) i++;
	count = i;

	array_init(return_value);

	add_assoc_long(return_value, "count", count);
	for (i = 0; i<count; i++) {
		add_index_string(return_value, i, ldap_value[i], 1);
	}

	ldap_memvfree((void **)ldap_value);
}