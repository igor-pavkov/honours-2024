static void php_ldap_do_search(INTERNAL_FUNCTION_PARAMETERS, int scope)
{
	zval *link, *base_dn, **filter, *attrs = NULL, **attr;
	long attrsonly, sizelimit, timelimit, deref;
	char *ldap_base_dn = NULL, *ldap_filter = NULL, **ldap_attrs = NULL;
	ldap_linkdata *ld = NULL;
	LDAPMessage *ldap_res;
	int ldap_attrsonly = 0, ldap_sizelimit = -1, ldap_timelimit = -1, ldap_deref = -1;
	int old_ldap_sizelimit = -1, old_ldap_timelimit = -1, old_ldap_deref = -1;
	int num_attribs = 0, ret = 1, i, errno, argcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argcount TSRMLS_CC, "zzZ|allll", &link, &base_dn, &filter, &attrs, &attrsonly,
		&sizelimit, &timelimit, &deref) == FAILURE) {
		return;
	}

	/* Reverse -> fall through */
	switch (argcount) {
		case 8:
			ldap_deref = deref;
		case 7:
			ldap_timelimit = timelimit;
		case 6:
			ldap_sizelimit = sizelimit;
		case 5:
			ldap_attrsonly = attrsonly;
		case 4:
			num_attribs = zend_hash_num_elements(Z_ARRVAL_P(attrs));
			ldap_attrs = safe_emalloc((num_attribs+1), sizeof(char *), 0);

			for (i = 0; i<num_attribs; i++) {
				if (zend_hash_index_find(Z_ARRVAL_P(attrs), i, (void **) &attr) != SUCCESS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array initialization wrong");
					ret = 0;
					goto cleanup;
				}

				SEPARATE_ZVAL(attr);
				convert_to_string_ex(attr);
				ldap_attrs[i] = Z_STRVAL_PP(attr);
			}
			ldap_attrs[num_attribs] = NULL;
		default:
			break;
	}

	/* parallel search? */
	if (Z_TYPE_P(link) == IS_ARRAY) {
		int i, nlinks, nbases, nfilters, *rcs;
		ldap_linkdata **lds;
		zval **entry, *resource;

		nlinks = zend_hash_num_elements(Z_ARRVAL_P(link));
		if (nlinks == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No links in link array");
			ret = 0;
			goto cleanup;
		}

		if (Z_TYPE_P(base_dn) == IS_ARRAY) {
			nbases = zend_hash_num_elements(Z_ARRVAL_P(base_dn));
			if (nbases != nlinks) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Base must either be a string, or an array with the same number of elements as the links array");
				ret = 0;
				goto cleanup;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(base_dn));
		} else {
			nbases = 0; /* this means string, not array */
			/* If anything else than string is passed, ldap_base_dn = NULL */
			if (Z_TYPE_P(base_dn) == IS_STRING) {
				ldap_base_dn = Z_STRVAL_P(base_dn);
			} else {
				ldap_base_dn = NULL;
			}
		}

		if (Z_TYPE_PP(filter) == IS_ARRAY) {
			nfilters = zend_hash_num_elements(Z_ARRVAL_PP(filter));
			if (nfilters != nlinks) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter must either be a string, or an array with the same number of elements as the links array");
				ret = 0;
				goto cleanup;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(filter));
		} else {
			nfilters = 0; /* this means string, not array */
			convert_to_string_ex(filter);
			ldap_filter = Z_STRVAL_PP(filter);
		}

		lds = safe_emalloc(nlinks, sizeof(ldap_linkdata), 0);
		rcs = safe_emalloc(nlinks, sizeof(*rcs), 0);

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(link));
		for (i=0; i<nlinks; i++) {
			zend_hash_get_current_data(Z_ARRVAL_P(link), (void **)&entry);

			ld = (ldap_linkdata *) zend_fetch_resource(entry TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);
			if (ld == NULL) {
				ret = 0;
				goto cleanup_parallel;
			}
			if (nbases != 0) { /* base_dn an array? */
				zend_hash_get_current_data(Z_ARRVAL_P(base_dn), (void **)&entry);
				zend_hash_move_forward(Z_ARRVAL_P(base_dn));

				/* If anything else than string is passed, ldap_base_dn = NULL */
				if (Z_TYPE_PP(entry) == IS_STRING) {
					ldap_base_dn = Z_STRVAL_PP(entry);
				} else {
					ldap_base_dn = NULL;
				}
			}
			if (nfilters != 0) { /* filter an array? */
				zend_hash_get_current_data(Z_ARRVAL_PP(filter), (void **)&entry);
				zend_hash_move_forward(Z_ARRVAL_PP(filter));
				convert_to_string_ex(entry);
				ldap_filter = Z_STRVAL_PP(entry);
			}

			php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref, &old_ldap_sizelimit, &old_ldap_timelimit, &old_ldap_deref);

			/* Run the actual search */
			ldap_search_ext(ld->link, ldap_base_dn, scope, ldap_filter, ldap_attrs, ldap_attrsonly, NULL, NULL, NULL, ldap_sizelimit, &rcs[i]);
			lds[i] = ld;
			zend_hash_move_forward(Z_ARRVAL_P(link));
		}

		array_init(return_value);

		/* Collect results from the searches */
		for (i=0; i<nlinks; i++) {
			MAKE_STD_ZVAL(resource);
			if (rcs[i] != -1) {
				rcs[i] = ldap_result(lds[i]->link, LDAP_RES_ANY, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
			}
			if (rcs[i] != -1) {
				ZEND_REGISTER_RESOURCE(resource, ldap_res, le_result);
				add_next_index_zval(return_value, resource);
			} else {
				add_next_index_bool(return_value, 0);
			}
		}

cleanup_parallel:
		efree(lds);
		efree(rcs);
	} else {
		convert_to_string_ex(filter);
		ldap_filter = Z_STRVAL_PP(filter);

		/* If anything else than string is passed, ldap_base_dn = NULL */
		if (Z_TYPE_P(base_dn) == IS_STRING) {
			ldap_base_dn = Z_STRVAL_P(base_dn);
		}

		ld = (ldap_linkdata *) zend_fetch_resource(&link TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);
		if (ld == NULL) {
			ret = 0;
			goto cleanup;
		}

		php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref, &old_ldap_sizelimit, &old_ldap_timelimit, &old_ldap_deref);

		/* Run the actual search */
		errno = ldap_search_ext_s(ld->link, ldap_base_dn, scope, ldap_filter, ldap_attrs, ldap_attrsonly, NULL, NULL, NULL, ldap_sizelimit, &ldap_res);

		if (errno != LDAP_SUCCESS
			&& errno != LDAP_SIZELIMIT_EXCEEDED
#ifdef LDAP_ADMINLIMIT_EXCEEDED
			&& errno != LDAP_ADMINLIMIT_EXCEEDED
#endif
#ifdef LDAP_REFERRAL
			&& errno != LDAP_REFERRAL
#endif
		) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Search: %s", ldap_err2string(errno));
			ret = 0;
		} else {
			if (errno == LDAP_SIZELIMIT_EXCEEDED) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Partial search results returned: Sizelimit exceeded");
			}
#ifdef LDAP_ADMINLIMIT_EXCEEDED
			else if (errno == LDAP_ADMINLIMIT_EXCEEDED) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Partial search results returned: Adminlimit exceeded");
			}
#endif

			ZEND_REGISTER_RESOURCE(return_value, ldap_res, le_result);
		}
	}

cleanup:
	if (ld) {
		/* Restoring previous options */
		php_set_opts(ld->link, old_ldap_sizelimit, old_ldap_timelimit, old_ldap_deref, &ldap_sizelimit, &ldap_timelimit, &ldap_deref);
	}
	if (ldap_attrs != NULL) {
		efree(ldap_attrs);
	}
	if (!ret) {
		RETVAL_BOOL(ret);
	}
}