static int php_openssl_make_REQ(struct php_x509_request * req, X509_REQ * csr, zval * dn, zval * attribs)
{
	STACK_OF(CONF_VALUE) * dn_sk, *attr_sk = NULL;
	char * str, *dn_sect, *attr_sect;

	dn_sect = CONF_get_string(req->req_config, req->section_name, "distinguished_name");
	if (dn_sect == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}
	dn_sk = CONF_get_section(req->req_config, dn_sect);
	if (dn_sk == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}
	attr_sect = CONF_get_string(req->req_config, req->section_name, "attributes");
	if (attr_sect == NULL) {
		php_openssl_store_errors();
		attr_sk = NULL;
	} else {
		attr_sk = CONF_get_section(req->req_config, attr_sect);
		if (attr_sk == NULL) {
			php_openssl_store_errors();
			return FAILURE;
		}
	}
	/* setup the version number: version 1 */
	if (X509_REQ_set_version(csr, 0L)) {
		int i, nid;
		char * type;
		CONF_VALUE * v;
		X509_NAME * subj;
		zval * item;
		zend_string * strindex = NULL;

		subj = X509_REQ_get_subject_name(csr);
		/* apply values from the dn hash */
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(dn), strindex, item) {
			if (strindex) {
				int nid;

				convert_to_string_ex(item);

				nid = OBJ_txt2nid(ZSTR_VAL(strindex));
				if (nid != NID_undef) {
					if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_UTF8,
								(unsigned char*)Z_STRVAL_P(item), -1, -1, 0))
					{
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING,
							"dn: add_entry_by_NID %d -> %s (failed; check error"
							" queue and value of string_mask OpenSSL option "
							"if illegal characters are reported)",
							nid, Z_STRVAL_P(item));
						return FAILURE;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "dn: %s is not a recognized name", ZSTR_VAL(strindex));
				}
			}
		} ZEND_HASH_FOREACH_END();

		/* Finally apply defaults from config file */
		for(i = 0; i < sk_CONF_VALUE_num(dn_sk); i++) {
			size_t len;
			char buffer[200 + 1]; /*200 + \0 !*/

			v = sk_CONF_VALUE_value(dn_sk, i);
			type = v->name;

			len = strlen(type);
			if (len < sizeof("_default")) {
				continue;
			}
			len -= sizeof("_default") - 1;
			if (strcmp("_default", type + len) != 0) {
				continue;
			}
			if (len > 200) {
				len = 200;
			}
			memcpy(buffer, type, len);
			buffer[len] = '\0';
			type = buffer;

			/* Skip past any leading X. X: X, etc to allow for multiple
			 * instances */
			for (str = type; *str; str++) {
				if (*str == ':' || *str == ',' || *str == '.') {
					str++;
					if (*str) {
						type = str;
					}
					break;
				}
			}
			/* if it is already set, skip this */
			nid = OBJ_txt2nid(type);
			if (X509_NAME_get_index_by_NID(subj, nid, -1) >= 0) {
				continue;
			}
			if (!X509_NAME_add_entry_by_txt(subj, type, MBSTRING_UTF8, (unsigned char*)v->value, -1, -1, 0)) {
				php_openssl_store_errors();
				php_error_docref(NULL, E_WARNING, "add_entry_by_txt %s -> %s (failed)", type, v->value);
				return FAILURE;
			}
			if (!X509_NAME_entry_count(subj)) {
				php_error_docref(NULL, E_WARNING, "no objects specified in config file");
				return FAILURE;
			}
		}
		if (attribs) {
			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(attribs), strindex, item) {
				int nid;

				if (NULL == strindex) {
					php_error_docref(NULL, E_WARNING, "dn: numeric fild names are not supported");
					continue;
				}

				convert_to_string_ex(item);

				nid = OBJ_txt2nid(ZSTR_VAL(strindex));
				if (nid != NID_undef) {
					if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_UTF8, (unsigned char*)Z_STRVAL_P(item), -1, -1, 0)) {
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING, "attribs: add_entry_by_NID %d -> %s (failed)", nid, Z_STRVAL_P(item));
						return FAILURE;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "dn: %s is not a recognized name", ZSTR_VAL(strindex));
				}
			} ZEND_HASH_FOREACH_END();
			for (i = 0; i < sk_CONF_VALUE_num(attr_sk); i++) {
				v = sk_CONF_VALUE_value(attr_sk, i);
				/* if it is already set, skip this */
				nid = OBJ_txt2nid(v->name);
				if (X509_REQ_get_attr_by_NID(csr, nid, -1) >= 0) {
					continue;
				}
				if (!X509_REQ_add1_attr_by_txt(csr, v->name, MBSTRING_UTF8, (unsigned char*)v->value, -1)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING,
						"add1_attr_by_txt %s -> %s (failed; check error queue "
						"and value of string_mask OpenSSL option if illegal "
						"characters are reported)",
						v->name, v->value);
					return FAILURE;
				}
			}
		}
	} else {
		php_openssl_store_errors();
	}

	if (!X509_REQ_set_pubkey(csr, req->priv_key)) {
		php_openssl_store_errors();
	}
	return SUCCESS;
}