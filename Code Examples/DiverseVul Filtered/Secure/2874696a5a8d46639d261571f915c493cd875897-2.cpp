static inline int php_openssl_config_check_syntax(const char * section_label, const char * config_filename, const char * section, LHASH * config TSRMLS_DC)
#endif
{
	X509V3_CTX ctx;

	X509V3_set_ctx_test(&ctx);
	X509V3_set_conf_lhash(&ctx, config);
	if (!X509V3_EXT_add_conf(config, &ctx, (char *)section, NULL)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error loading %s section %s of %s",
				section_label,
				section,
				config_filename);
		return FAILURE;
	}
	return SUCCESS;
}