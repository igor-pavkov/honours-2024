PHP_FUNCTION(openssl_pkey_new)
{
	struct php_x509_request req;
	zval * args = NULL;
	zval **data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a!", &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (args && Z_TYPE_P(args) == IS_ARRAY) {
		EVP_PKEY *pkey;

		if (zend_hash_find(Z_ARRVAL_P(args), "rsa", sizeof("rsa"), (void**)&data) == SUCCESS &&
		    Z_TYPE_PP(data) == IS_ARRAY) {
		    pkey = EVP_PKEY_new();
		    if (pkey) {
				RSA *rsa = RSA_new();
				if (rsa) {
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, n);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, e);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, d);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, p);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, q);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, dmp1);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, dmq1);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, iqmp);
					if (rsa->n && rsa->d) {
						if (EVP_PKEY_assign_RSA(pkey, rsa)) {
							RETURN_RESOURCE(zend_list_insert(pkey, le_key TSRMLS_CC));
						}
					}
					RSA_free(rsa);
				}
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		} else if (zend_hash_find(Z_ARRVAL_P(args), "dsa", sizeof("dsa"), (void**)&data) == SUCCESS &&
		           Z_TYPE_PP(data) == IS_ARRAY) {
		    pkey = EVP_PKEY_new();
		    if (pkey) {
				DSA *dsa = DSA_new();
				if (dsa) {
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, p);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, q);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, g);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, priv_key);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, pub_key);
					if (php_openssl_pkey_init_dsa(dsa)) {
						if (EVP_PKEY_assign_DSA(pkey, dsa)) {
							RETURN_RESOURCE(zend_list_insert(pkey, le_key TSRMLS_CC));
						}
					}
					DSA_free(dsa);
				}
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		} else if (zend_hash_find(Z_ARRVAL_P(args), "dh", sizeof("dh"), (void**)&data) == SUCCESS &&
		           Z_TYPE_PP(data) == IS_ARRAY) {
		    pkey = EVP_PKEY_new();
		    if (pkey) {
				DH *dh = DH_new();
				if (dh) {
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, p);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, g);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, priv_key);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, pub_key);
					if (php_openssl_pkey_init_dh(dh)) {
						if (EVP_PKEY_assign_DH(pkey, dh)) {
							RETURN_RESOURCE(zend_list_insert(pkey, le_key TSRMLS_CC));
						}
					}
					DH_free(dh);
				}
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		}
	} 

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS)
	{
		if (php_openssl_generate_private_key(&req TSRMLS_CC)) {
			/* pass back a key resource */
			RETVAL_RESOURCE(zend_list_insert(req.priv_key, le_key TSRMLS_CC));
			/* make sure the cleanup code doesn't zap it! */
			req.priv_key = NULL;
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);
}