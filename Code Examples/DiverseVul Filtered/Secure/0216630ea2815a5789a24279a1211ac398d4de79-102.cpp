static STACK_OF(X509) * php_array_to_X509_sk(zval * zcerts) /* {{{ */
{
	zval * zcertval;
	STACK_OF(X509) * sk = NULL;
	X509 * cert;
	zend_resource *certresource;

	sk = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zcerts), zcertval) {
			cert = php_openssl_x509_from_zval(zcertval, 0, &certresource);
			if (cert == NULL) {
				goto clean_exit;
			}

			if (certresource != NULL) {
				cert = X509_dup(cert);

				if (cert == NULL) {
					php_openssl_store_errors();
					goto clean_exit;
				}

			}
			sk_X509_push(sk, cert);
		} ZEND_HASH_FOREACH_END();
	} else {
		/* a single certificate */
		cert = php_openssl_x509_from_zval(zcerts, 0, &certresource);

		if (cert == NULL) {
			goto clean_exit;
		}

		if (certresource != NULL) {
			cert = X509_dup(cert);
			if (cert == NULL) {
				php_openssl_store_errors();
				goto clean_exit;
			}
		}
		sk_X509_push(sk, cert);
	}

clean_exit:
	return sk;
}