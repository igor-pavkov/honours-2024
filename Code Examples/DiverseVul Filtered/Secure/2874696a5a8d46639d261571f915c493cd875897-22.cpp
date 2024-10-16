static STACK_OF(X509) * php_array_to_X509_sk(zval ** zcerts TSRMLS_DC) /* {{{ */
{
	HashPosition hpos;
	zval ** zcertval;
	STACK_OF(X509) * sk = NULL;
    X509 * cert;
    long certresource;

	sk = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_PP(zcerts) == IS_ARRAY) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(*zcerts), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(*zcerts), (void**)&zcertval, &hpos) == SUCCESS) {

			cert = php_openssl_x509_from_zval(zcertval, 0, &certresource TSRMLS_CC);
			if (cert == NULL) {
				goto clean_exit;
			}

			if (certresource != -1) {
				cert = X509_dup(cert);

				if (cert == NULL) {
					goto clean_exit;
				}

			}
			sk_X509_push(sk, cert);

			zend_hash_move_forward_ex(HASH_OF(*zcerts), &hpos);
		}
	} else {
		/* a single certificate */
		cert = php_openssl_x509_from_zval(zcerts, 0, &certresource TSRMLS_CC);

		if (cert == NULL) {
			goto clean_exit;
		}

		if (certresource != -1) {
			cert = X509_dup(cert);
			if (cert == NULL) {
				goto clean_exit;
			}
		}
		sk_X509_push(sk, cert);
	}

  clean_exit:
    return sk;
}