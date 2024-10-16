zend_bool php_openssl_pkey_init_dh(DH *dh)
{
	if (!dh->p || !dh->g) {
		return 0;
	}
	if (dh->pub_key) {
		return 1;
	}
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DH_generate_key(dh)) {
		return 0;
	}
	/* all good */
	return 1;
}