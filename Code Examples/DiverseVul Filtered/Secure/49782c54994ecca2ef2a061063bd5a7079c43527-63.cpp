static void _ldap_hash_fetch(zval *hashTbl, const char *key, zval **out)
{
	zval **fetched;
	if (zend_hash_find(Z_ARRVAL_P(hashTbl), key, strlen(key)+1, (void **) &fetched) == SUCCESS) {
		*out = *fetched;
	}
	else {
		*out = NULL;
	}
}