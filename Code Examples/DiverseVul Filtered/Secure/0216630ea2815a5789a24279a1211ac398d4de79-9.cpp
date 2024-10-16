static void php_openssl_add_method_or_alias(const OBJ_NAME *name, void *arg) /* {{{ */
{
	add_next_index_string((zval*)arg, (char*)name->name);
}