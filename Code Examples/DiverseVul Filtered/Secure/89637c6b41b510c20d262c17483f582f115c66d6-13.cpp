static int php_openssl_is_private_key(EVP_PKEY* pkey TSRMLS_DC)
{
	assert(pkey != NULL);

	switch (pkey->type) {
#ifndef NO_RSA
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			assert(pkey->pkey.rsa != NULL);
			if (pkey->pkey.rsa != NULL && (NULL == pkey->pkey.rsa->p || NULL == pkey->pkey.rsa->q)) {
				return 0;
			}
			break;
#endif
#ifndef NO_DSA
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA1:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			assert(pkey->pkey.dsa != NULL);

			if (NULL == pkey->pkey.dsa->p || NULL == pkey->pkey.dsa->q || NULL == pkey->pkey.dsa->priv_key){ 
				return 0;
			}
			break;
#endif
#ifndef NO_DH
		case EVP_PKEY_DH:
			assert(pkey->pkey.dh != NULL);

			if (NULL == pkey->pkey.dh->p || NULL == pkey->pkey.dh->priv_key) {
				return 0;
			}
			break;
#endif
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			assert(pkey->pkey.ec != NULL);

			if ( NULL == EC_KEY_get0_private_key(pkey->pkey.ec)) {
				return 0;
			}
			break;
#endif
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
			break;
	}
	return 1;
}