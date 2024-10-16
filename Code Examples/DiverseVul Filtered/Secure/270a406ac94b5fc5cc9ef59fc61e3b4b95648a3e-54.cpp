static int add_oid_section(struct php_x509_request * req TSRMLS_DC) /* {{{ */
{
	char * str;
	STACK_OF(CONF_VALUE) * sktmp;
	CONF_VALUE * cnf;
	int i;

	str = CONF_get_string(req->req_config, NULL, "oid_section");
	if (str == NULL) {
		return SUCCESS;
	}
	sktmp = CONF_get_section(req->req_config, str);
	if (sktmp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "problem loading oid section %s", str);
		return FAILURE;
	}
	for (i = 0; i < sk_CONF_VALUE_num(sktmp); i++) {
		cnf = sk_CONF_VALUE_value(sktmp, i);
		if (OBJ_create(cnf->value, cnf->name, cnf->name) == NID_undef) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "problem creating object %s=%s", cnf->name, cnf->value);
			return FAILURE;
		}
	}
	return SUCCESS;
}