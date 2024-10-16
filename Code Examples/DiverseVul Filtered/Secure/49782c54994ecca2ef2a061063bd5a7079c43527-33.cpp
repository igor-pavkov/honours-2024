PHP_FUNCTION(ldap_modify_batch)
{
	ldap_linkdata *ld;
	zval *link, *mods, *mod, *modinfo, *modval;
	zval *attrib, *modtype, *vals;
	zval **fetched;
	char *dn;
	int dn_len;
	int i, j, k;
	int num_mods, num_modprops, num_modvals;
	LDAPMod **ldap_mods;
	uint oper;

	/*
	$mods = array(
		array(
			"attrib" => "unicodePwd",
			"modtype" => LDAP_MODIFY_BATCH_REMOVE,
			"values" => array($oldpw)
		),
		array(
			"attrib" => "unicodePwd",
			"modtype" => LDAP_MODIFY_BATCH_ADD,
			"values" => array($newpw)
		),
		array(
			"attrib" => "userPrincipalName",
			"modtype" => LDAP_MODIFY_BATCH_REPLACE,
			"values" => array("janitor@corp.contoso.com")
		),
		array(
			"attrib" => "userCert",
			"modtype" => LDAP_MODIFY_BATCH_REMOVE_ALL
		)
	);
	*/

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsa", &link, &dn, &dn_len, &mods) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	/* perform validation */
	{
		char *modkey;
		uint modkeylen;
		long modtype;

		/* to store the wrongly-typed keys */
		ulong tmpUlong;

		/* make sure the DN contains no NUL bytes */
		if (_ldap_strlen_max(dn, dn_len) != dn_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "DN must not contain NUL bytes");
			RETURN_FALSE;
		}

		/* make sure the top level is a normal array */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(mods));
		if (zend_hash_get_current_key_type(Z_ARRVAL_P(mods)) != HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modifications array must not be string-indexed");
			RETURN_FALSE;
		}

		num_mods = zend_hash_num_elements(Z_ARRVAL_P(mods));

		for (i = 0; i < num_mods; i++) {
			/* is the numbering consecutive? */
			if (zend_hash_index_find(Z_ARRVAL_P(mods), i, (void **) &fetched) != SUCCESS) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modifications array must have consecutive indices 0, 1, ...");
				RETURN_FALSE;
			}
			mod = *fetched;

			/* is it an array? */
			if (Z_TYPE_P(mod) != IS_ARRAY) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Each entry of modifications array must be an array itself");
				RETURN_FALSE;
			}

			/* for the modification hashtable... */
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(mod));
			num_modprops = zend_hash_num_elements(Z_ARRVAL_P(mod));

			for (j = 0; j < num_modprops; j++) {
				/* are the keys strings? */
				if (zend_hash_get_current_key_ex(Z_ARRVAL_P(mod), &modkey, &modkeylen, &tmpUlong, 0, NULL) != HASH_KEY_IS_STRING) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Each entry of modifications array must be string-indexed");
					RETURN_FALSE;
				}

				/* modkeylen includes the terminating NUL byte; remove that */
				--modkeylen;

				/* is this a valid entry? */
				if (
					!_ldap_str_equal_to_const(modkey, modkeylen, LDAP_MODIFY_BATCH_ATTRIB) &&
					!_ldap_str_equal_to_const(modkey, modkeylen, LDAP_MODIFY_BATCH_MODTYPE) &&
					!_ldap_str_equal_to_const(modkey, modkeylen, LDAP_MODIFY_BATCH_VALUES)
				) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "The only allowed keys in entries of the modifications array are '" LDAP_MODIFY_BATCH_ATTRIB "', '" LDAP_MODIFY_BATCH_MODTYPE "' and '" LDAP_MODIFY_BATCH_VALUES "'");
					RETURN_FALSE;
				}

				zend_hash_get_current_data(Z_ARRVAL_P(mod), (void **) &fetched);
				modinfo = *fetched;

				/* does the value type match the key? */
				if (_ldap_str_equal_to_const(modkey, modkeylen, LDAP_MODIFY_BATCH_ATTRIB)) {
					if (Z_TYPE_P(modinfo) != IS_STRING) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_ATTRIB "' value must be a string");
						RETURN_FALSE;
					}

					if (Z_STRLEN_P(modinfo) != _ldap_strlen_max(Z_STRVAL_P(modinfo), Z_STRLEN_P(modinfo))) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_ATTRIB "' value must not contain NUL bytes");
						RETURN_FALSE;
					}
				}
				else if (_ldap_str_equal_to_const(modkey, modkeylen, LDAP_MODIFY_BATCH_MODTYPE)) {
					if (Z_TYPE_P(modinfo) != IS_LONG) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_MODTYPE "' value must be a long");
						RETURN_FALSE;
					}

					/* is the value in range? */
					modtype = Z_LVAL_P(modinfo);
					if (
						modtype != LDAP_MODIFY_BATCH_ADD &&
						modtype != LDAP_MODIFY_BATCH_REMOVE &&
						modtype != LDAP_MODIFY_BATCH_REPLACE &&
						modtype != LDAP_MODIFY_BATCH_REMOVE_ALL
					) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "The '" LDAP_MODIFY_BATCH_MODTYPE "' value must match one of the LDAP_MODIFY_BATCH_* constants");
						RETURN_FALSE;
					}

					/* if it's REMOVE_ALL, there must not be a values array; otherwise, there must */
					if (modtype == LDAP_MODIFY_BATCH_REMOVE_ALL) {
						if (zend_hash_exists(Z_ARRVAL_P(mod), LDAP_MODIFY_BATCH_VALUES, strlen(LDAP_MODIFY_BATCH_VALUES) + 1)) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "If '" LDAP_MODIFY_BATCH_MODTYPE "' is LDAP_MODIFY_BATCH_REMOVE_ALL, a '" LDAP_MODIFY_BATCH_VALUES "' array must not be provided");
							RETURN_FALSE;
						}
					}
					else {
						if (!zend_hash_exists(Z_ARRVAL_P(mod), LDAP_MODIFY_BATCH_VALUES, strlen(LDAP_MODIFY_BATCH_VALUES) + 1)) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "If '" LDAP_MODIFY_BATCH_MODTYPE "' is not LDAP_MODIFY_BATCH_REMOVE_ALL, a '" LDAP_MODIFY_BATCH_VALUES "' array must be provided");
							RETURN_FALSE;
						}
					}
				}
				else if (_ldap_str_equal_to_const(modkey, modkeylen, LDAP_MODIFY_BATCH_VALUES)) {
					if (Z_TYPE_P(modinfo) != IS_ARRAY) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' value must be an array");
						RETURN_FALSE;
					}

					/* is the array not empty? */
					zend_hash_internal_pointer_reset(Z_ARRVAL_P(modinfo));
					num_modvals = zend_hash_num_elements(Z_ARRVAL_P(modinfo));
					if (num_modvals == 0) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' array must have at least one element");
						RETURN_FALSE;
					}

					/* are its keys integers? */
					if (zend_hash_get_current_key_type(Z_ARRVAL_P(modinfo)) != HASH_KEY_IS_LONG) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' array must not be string-indexed");
						RETURN_FALSE;
					}

					/* are the keys consecutive? */
					for (k = 0; k < num_modvals; k++) {
						if (zend_hash_index_find(Z_ARRVAL_P(modinfo), k, (void **) &fetched) != SUCCESS) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' array must have consecutive indices 0, 1, ...");
							RETURN_FALSE;
						}
						modval = *fetched;

						/* is the data element a string? */
						if (Z_TYPE_P(modval) != IS_STRING) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Each element of a '" LDAP_MODIFY_BATCH_VALUES "' array must be a string");
							RETURN_FALSE;
						}
					}
				}

				zend_hash_move_forward(Z_ARRVAL_P(mod));
			}
		}
	}
	/* validation was successful */

	/* allocate array of modifications */
	ldap_mods = safe_emalloc((num_mods+1), sizeof(LDAPMod *), 0);

	/* for each modification */
	for (i = 0; i < num_mods; i++) {
		/* allocate the modification struct */
		ldap_mods[i] = safe_emalloc(1, sizeof(LDAPMod), 0);

		/* fetch the relevant data */
		zend_hash_index_find(Z_ARRVAL_P(mods), i, (void **) &fetched);
		mod = *fetched;

		_ldap_hash_fetch(mod, LDAP_MODIFY_BATCH_ATTRIB, &attrib);
		_ldap_hash_fetch(mod, LDAP_MODIFY_BATCH_MODTYPE, &modtype);
		_ldap_hash_fetch(mod, LDAP_MODIFY_BATCH_VALUES, &vals);

		/* map the modification type */
		switch (Z_LVAL_P(modtype)) {
			case LDAP_MODIFY_BATCH_ADD:
				oper = LDAP_MOD_ADD;
				break;
			case LDAP_MODIFY_BATCH_REMOVE:
			case LDAP_MODIFY_BATCH_REMOVE_ALL:
				oper = LDAP_MOD_DELETE;
				break;
			case LDAP_MODIFY_BATCH_REPLACE:
				oper = LDAP_MOD_REPLACE;
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown and uncaught modification type.");
				RETURN_FALSE;
		}

		/* fill in the basic info */
		ldap_mods[i]->mod_op = oper | LDAP_MOD_BVALUES;
		ldap_mods[i]->mod_type = estrndup(Z_STRVAL_P(attrib), Z_STRLEN_P(attrib));

		if (Z_LVAL_P(modtype) == LDAP_MODIFY_BATCH_REMOVE_ALL) {
			/* no values */
			ldap_mods[i]->mod_bvalues = NULL;
		}
		else {
			/* allocate space for the values as part of this modification */
			num_modvals = zend_hash_num_elements(Z_ARRVAL_P(vals));
			ldap_mods[i]->mod_bvalues = safe_emalloc((num_modvals+1), sizeof(struct berval *), 0);

			/* for each value */
			for (j = 0; j < num_modvals; j++) {
				/* fetch it */
				zend_hash_index_find(Z_ARRVAL_P(vals), j, (void **) &fetched);
				modval = *fetched;

				/* allocate the data struct */
				ldap_mods[i]->mod_bvalues[j] = safe_emalloc(1, sizeof(struct berval), 0);

				/* fill it */
				ldap_mods[i]->mod_bvalues[j]->bv_len = Z_STRLEN_P(modval);
				ldap_mods[i]->mod_bvalues[j]->bv_val = estrndup(Z_STRVAL_P(modval), Z_STRLEN_P(modval));
			}

			/* NULL-terminate values */
			ldap_mods[i]->mod_bvalues[num_modvals] = NULL;
		}
	}

	/* NULL-terminate modifications */
	ldap_mods[num_mods] = NULL;

	/* perform (finally) */
	if ((i = ldap_modify_ext_s(ld->link, dn, ldap_mods, NULL, NULL)) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Batch Modify: %s", ldap_err2string(i));
		RETVAL_FALSE;
	} else RETVAL_TRUE;

	/* clean up */
	{
		for (i = 0; i < num_mods; i++) {
			/* attribute */
			efree(ldap_mods[i]->mod_type);

			if (ldap_mods[i]->mod_bvalues != NULL) {
				/* each BER value */
				for (j = 0; ldap_mods[i]->mod_bvalues[j] != NULL; j++) {
					/* free the data bytes */
					efree(ldap_mods[i]->mod_bvalues[j]->bv_val);

					/* free the bvalue struct */
					efree(ldap_mods[i]->mod_bvalues[j]);
				}

				/* the BER value array */
				efree(ldap_mods[i]->mod_bvalues);
			}

			/* the modification */
			efree(ldap_mods[i]);
		}

		/* the modifications array */
		efree(ldap_mods);
	}
}