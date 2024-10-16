PHP_FUNCTION(ldap_get_attributes)
{
	zval *link, *result_entry;
	zval *tmp;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	struct berval **ldap_value;
	int i, num_values, num_attrib;
	BerElement *ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	array_init(return_value);
	num_attrib = 0;

	attribute = ldap_first_attribute(ld->link, resultentry->data, &ber);
	while (attribute != NULL) {
		ldap_value = ldap_get_values_len(ld->link, resultentry->data, attribute);
		num_values = ldap_count_values_len(ldap_value);

		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		add_assoc_long(tmp, "count", num_values);
		for (i = 0; i < num_values; i++) {
			add_index_stringl(tmp, i, ldap_value[i]->bv_val, ldap_value[i]->bv_len, 1);
		}
		ldap_value_free_len(ldap_value);

		zend_hash_update(Z_ARRVAL_P(return_value), attribute, strlen(attribute)+1, (void *) &tmp, sizeof(zval *), NULL);
		add_index_string(return_value, num_attrib, attribute, 1);

		num_attrib++;
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP || WINDOWS
		ldap_memfree(attribute);
#endif
		attribute = ldap_next_attribute(ld->link, resultentry->data, ber);
	}
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP || WINDOWS
	if (ber != NULL) {
		ber_free(ber, 0);
	}
#endif

	add_assoc_long(return_value, "count", num_attrib);
}