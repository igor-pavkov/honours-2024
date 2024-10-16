static void php_ldap_do_modify(INTERNAL_FUNCTION_PARAMETERS, int oper)
{
	zval *link, *entry, **value, **ivalue;
	ldap_linkdata *ld;
	char *dn;
	LDAPMod **ldap_mods;
	int i, j, num_attribs, num_values, dn_len;
	int *num_berval;
	char *attribute;
	ulong index;
	int is_full_add=0; /* flag for full add operation so ldap_mod_add can be put back into oper, gerrit THomson */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsa", &link, &dn, &dn_len, &entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	num_attribs = zend_hash_num_elements(Z_ARRVAL_P(entry));
	ldap_mods = safe_emalloc((num_attribs+1), sizeof(LDAPMod *), 0);
	num_berval = safe_emalloc(num_attribs, sizeof(int), 0);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(entry));

	/* added by gerrit thomson to fix ldap_add using ldap_mod_add */
	if (oper == PHP_LD_FULL_ADD) {
		oper = LDAP_MOD_ADD;
		is_full_add = 1;
	}
	/* end additional , gerrit thomson */

	for (i = 0; i < num_attribs; i++) {
		ldap_mods[i] = emalloc(sizeof(LDAPMod));
		ldap_mods[i]->mod_op = oper | LDAP_MOD_BVALUES;
		ldap_mods[i]->mod_type = NULL;

		if (zend_hash_get_current_key(Z_ARRVAL_P(entry), &attribute, &index, 0) == HASH_KEY_IS_STRING) {
			ldap_mods[i]->mod_type = estrdup(attribute);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown attribute in the data");
			/* Free allocated memory */
			while (i >= 0) {
				if (ldap_mods[i]->mod_type) {
					efree(ldap_mods[i]->mod_type);
				}
				efree(ldap_mods[i]);
				i--;
			}
			efree(num_berval);
			efree(ldap_mods);
			RETURN_FALSE;
		}

		zend_hash_get_current_data(Z_ARRVAL_P(entry), (void **)&value);

		if (Z_TYPE_PP(value) != IS_ARRAY) {
			num_values = 1;
		} else {
			num_values = zend_hash_num_elements(Z_ARRVAL_PP(value));
		}

		num_berval[i] = num_values;
		ldap_mods[i]->mod_bvalues = safe_emalloc((num_values + 1), sizeof(struct berval *), 0);

/* allow for arrays with one element, no allowance for arrays with none but probably not required, gerrit thomson. */
		if ((num_values == 1) && (Z_TYPE_PP(value) != IS_ARRAY)) {
			convert_to_string_ex(value);
			ldap_mods[i]->mod_bvalues[0] = (struct berval *) emalloc (sizeof(struct berval));
			ldap_mods[i]->mod_bvalues[0]->bv_len = Z_STRLEN_PP(value);
			ldap_mods[i]->mod_bvalues[0]->bv_val = Z_STRVAL_PP(value);
		} else {
			for (j = 0; j < num_values; j++) {
				if (zend_hash_index_find(Z_ARRVAL_PP(value), j, (void **) &ivalue) != SUCCESS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value array must have consecutive indices 0, 1, ...");
					num_berval[i] = j;
					num_attribs = i + 1;
					RETVAL_FALSE;
					goto errexit;
				}
				convert_to_string_ex(ivalue);
				ldap_mods[i]->mod_bvalues[j] = (struct berval *) emalloc (sizeof(struct berval));
				ldap_mods[i]->mod_bvalues[j]->bv_len = Z_STRLEN_PP(ivalue);
				ldap_mods[i]->mod_bvalues[j]->bv_val = Z_STRVAL_PP(ivalue);
			}
		}
		ldap_mods[i]->mod_bvalues[num_values] = NULL;
		zend_hash_move_forward(Z_ARRVAL_P(entry));
	}
	ldap_mods[num_attribs] = NULL;

/* check flag to see if do_mod was called to perform full add , gerrit thomson */
	if (is_full_add == 1) {
		if ((i = ldap_add_ext_s(ld->link, dn, ldap_mods, NULL, NULL)) != LDAP_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Add: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else RETVAL_TRUE;
	} else {
		if ((i = ldap_modify_ext_s(ld->link, dn, ldap_mods, NULL, NULL)) != LDAP_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modify: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else RETVAL_TRUE;
	}

errexit:
	for (i = 0; i < num_attribs; i++) {
		efree(ldap_mods[i]->mod_type);
		for (j = 0; j < num_berval[i]; j++) {
			efree(ldap_mods[i]->mod_bvalues[j]);
		}
		efree(ldap_mods[i]->mod_bvalues);
		efree(ldap_mods[i]);
	}
	efree(num_berval);
	efree(ldap_mods);

	return;
}