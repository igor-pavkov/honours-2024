static inline int phar_find_key(HashTable *_SERVER, char *key, int len, void **stuff TSRMLS_DC)
{
	if (SUCCESS == zend_hash_find(_SERVER, key, len, stuff)) {
		return 1;
	} else {
		int s = len;
		zstr var;
		s = phar_make_unicode(&var, key, len TSRMLS_CC);
		if (SUCCESS == zend_u_hash_find(_SERVER, IS_UNICODE, var, s, stuff)) {
			efree(var.u);
			return 1;
		}
		efree(var.u);
		return 0;
	}
}