static int php_openssl_add_oid_section(struct php_x509_request * req) /* {{{ */
{
	char * str;
	STACK_OF(CONF_VALUE) * sktmp;
	CONF_VALUE * cnf;
	int i;

	str = CONF_get_string(req->req_config, NULL, "oid_section");
	if (str == NULL) {
		php_openssl_store_errors();
		return SUCCESS;
	}
	sktmp = CONF_get_section(req->req_config, str);
	if (sktmp == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "problem loading oid section %s", str);
		return FAILURE;
	}
	for (i = 0; i < sk_CONF_VALUE_num(sktmp); i++) {
		cnf = sk_CONF_VALUE_value(sktmp, i);
		if (OBJ_sn2nid(cnf->name) == NID_undef && OBJ_ln2nid(cnf->name) == NID_undef &&
				OBJ_create(cnf->value, cnf->name, cnf->name) == NID_undef) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "problem creating object %s=%s", cnf->name, cnf->value);
			return FAILURE;
		}
	}
	return SUCCESS;
}