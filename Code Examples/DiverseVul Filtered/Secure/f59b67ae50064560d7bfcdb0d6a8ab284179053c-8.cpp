static inline int phar_make_unicode(zstr *c_var, char *arKey, uint nKeyLength TSRMLS_DC)
{
	int c_var_len;
	UConverter *conv = ZEND_U_CONVERTER(UG(runtime_encoding_conv));

	c_var->u = NULL;
	if (zend_string_to_unicode(conv, &c_var->u, &c_var_len, arKey, nKeyLength TSRMLS_CC) == FAILURE) {

		if (c_var->u) {
			efree(c_var->u);
		}
		return 0;

	}
	return c_var_len;
}