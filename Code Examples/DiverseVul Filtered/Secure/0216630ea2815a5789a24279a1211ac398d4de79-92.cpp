static void php_openssl_add_method(const OBJ_NAME *name, void *arg) /* {{{ */
{
	if (name->alias == 0) {
		add_next_index_string((zval*)arg, (char*)name->name);
	}
}