static void php_apache_ini_dtor(request_rec *r, request_rec *p TSRMLS_DC)
{
	if (strcmp(r->protocol, "INCLUDED")) {
		zend_try { zend_ini_deactivate(TSRMLS_C); } zend_end_try();
	} else {
typedef struct {
	HashTable config;
} php_conf_rec;
		char *str;
		uint str_len;
		php_conf_rec *c = ap_get_module_config(r->per_dir_config, &php5_module);

		for (zend_hash_internal_pointer_reset(&c->config);
			zend_hash_get_current_key_ex(&c->config, &str, &str_len, NULL, 0,  NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&c->config)
		) {
			zend_restore_ini_entry(str, str_len, ZEND_INI_STAGE_SHUTDOWN);
		}
	}
	if (p) {
		((php_struct *)SG(server_context))->r = p;
	} else {
		apr_pool_cleanup_run(r->pool, (void *)&SG(server_context), php_server_context_cleanup);
	}
}