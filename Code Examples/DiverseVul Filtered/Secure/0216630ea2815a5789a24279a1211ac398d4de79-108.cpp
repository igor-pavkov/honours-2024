PHP_FUNCTION(openssl_pkey_new)
{
	struct php_x509_request req;
	zval * args = NULL;
	zval *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a!", &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (args && Z_TYPE_P(args) == IS_ARRAY) {
		EVP_PKEY *pkey;

		if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "rsa", sizeof("rsa")-1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = EVP_PKEY_new();
			if (pkey) {
				RSA *rsa = RSA_new();
				if (rsa) {
					if (php_openssl_pkey_init_and_assign_rsa(pkey, rsa, data)) {
						RETURN_RES(zend_register_resource(pkey, le_key));
					}
					RSA_free(rsa);
				} else {
					php_openssl_store_errors();
				}
				EVP_PKEY_free(pkey);
			} else {
				php_openssl_store_errors();
			}
			RETURN_FALSE;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "dsa", sizeof("dsa") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = EVP_PKEY_new();
			if (pkey) {
				DSA *dsa = DSA_new();
				if (dsa) {
					if (php_openssl_pkey_init_dsa(dsa, data)) {
						if (EVP_PKEY_assign_DSA(pkey, dsa)) {
							RETURN_RES(zend_register_resource(pkey, le_key));
						} else {
							php_openssl_store_errors();
						}
					}
					DSA_free(dsa);
				} else {
					php_openssl_store_errors();
				}
				EVP_PKEY_free(pkey);
			} else {
				php_openssl_store_errors();
			}
			RETURN_FALSE;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "dh", sizeof("dh") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = EVP_PKEY_new();
			if (pkey) {
				DH *dh = DH_new();
				if (dh) {
					if (php_openssl_pkey_init_dh(dh, data)) {
						if (EVP_PKEY_assign_DH(pkey, dh)) {
							ZVAL_COPY_VALUE(return_value, zend_list_insert(pkey, le_key));
							return;
						} else {
							php_openssl_store_errors();
						}
					}
					DH_free(dh);
				} else {
					php_openssl_store_errors();
				}
				EVP_PKEY_free(pkey);
			} else {
				php_openssl_store_errors();
			}
			RETURN_FALSE;
#ifdef HAVE_EVP_PKEY_EC
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "ec", sizeof("ec") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			EC_KEY *eckey = NULL;
			EC_GROUP *group = NULL;
			EC_POINT *pnt = NULL;
			BIGNUM *d = NULL;
			pkey = EVP_PKEY_new();
			if (pkey) {
				eckey = EC_KEY_new();
				if (eckey) {
					EC_GROUP *group = NULL;
					zval *bn;
					zval *x;
					zval *y;

					if ((bn = zend_hash_str_find(Z_ARRVAL_P(data), "curve_name", sizeof("curve_name") - 1)) != NULL &&
							Z_TYPE_P(bn) == IS_STRING) {
						int nid = OBJ_sn2nid(Z_STRVAL_P(bn));
						if (nid != NID_undef) {
							group = EC_GROUP_new_by_curve_name(nid);
							if (!group) {
								php_openssl_store_errors();
								goto clean_exit;
							}
							EC_GROUP_set_asn1_flag(group, OPENSSL_EC_NAMED_CURVE);
							EC_GROUP_set_point_conversion_form(group, POINT_CONVERSION_UNCOMPRESSED);
							if (!EC_KEY_set_group(eckey, group)) {
								php_openssl_store_errors();
								goto clean_exit;
							}
						}
					}

					if (group == NULL) {
						php_error_docref(NULL, E_WARNING, "Unknown curve_name");
						goto clean_exit;
					}

					// The public key 'pnt' can be calculated from 'd' or is defined by 'x' and 'y'
					if ((bn = zend_hash_str_find(Z_ARRVAL_P(data), "d", sizeof("d") - 1)) != NULL &&
							Z_TYPE_P(bn) == IS_STRING) {
						d = BN_bin2bn((unsigned char*) Z_STRVAL_P(bn), Z_STRLEN_P(bn), NULL);
						if (!EC_KEY_set_private_key(eckey, d)) {
							php_openssl_store_errors();
							goto clean_exit;
						}
						// Calculate the public key by multiplying the Point Q with the public key
						// P = d * Q
						pnt = EC_POINT_new(group);
						if (!pnt || !EC_POINT_mul(group, pnt, d, NULL, NULL, NULL)) {
							php_openssl_store_errors();
							goto clean_exit;
						}

						BN_free(d);
					} else if ((x = zend_hash_str_find(Z_ARRVAL_P(data), "x", sizeof("x") - 1)) != NULL &&
							Z_TYPE_P(x) == IS_STRING &&
							(y = zend_hash_str_find(Z_ARRVAL_P(data), "y", sizeof("y") - 1)) != NULL &&
							Z_TYPE_P(y) == IS_STRING) {
						pnt = EC_POINT_new(group);
						if (pnt == NULL) {
							php_openssl_store_errors();
							goto clean_exit;
						}
						if (!EC_POINT_set_affine_coordinates_GFp(
								group, pnt, BN_bin2bn((unsigned char*) Z_STRVAL_P(x), Z_STRLEN_P(x), NULL),
								BN_bin2bn((unsigned char*) Z_STRVAL_P(y), Z_STRLEN_P(y), NULL), NULL)) {
							php_openssl_store_errors();
							goto clean_exit;
						}
					}

					if (pnt != NULL) {
						if (!EC_KEY_set_public_key(eckey, pnt)) {
							php_openssl_store_errors();
							goto clean_exit;
						}
						EC_POINT_free(pnt);
						pnt = NULL;
					}

					if (!EC_KEY_check_key(eckey)) {
						PHP_OPENSSL_RAND_ADD_TIME();
						EC_KEY_generate_key(eckey);
						php_openssl_store_errors();
					}
					if (EC_KEY_check_key(eckey) && EVP_PKEY_assign_EC_KEY(pkey, eckey)) {
						EC_GROUP_free(group);
						RETURN_RES(zend_register_resource(pkey, le_key));
					} else {
						php_openssl_store_errors();
					}
				} else {
					php_openssl_store_errors();
				}
			} else {
				php_openssl_store_errors();
			}
clean_exit:
			if (d != NULL) {
				BN_free(d);
			}
			if (pnt != NULL) {
				EC_POINT_free(pnt);
			}
			if (group != NULL) {
				EC_GROUP_free(group);
			}
			if (eckey != NULL) {
				EC_KEY_free(eckey);
			}
			if (pkey != NULL) {
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
#endif
		}
	}

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		if (php_openssl_generate_private_key(&req)) {
			/* pass back a key resource */
			RETVAL_RES(zend_register_resource(req.priv_key, le_key));
			/* make sure the cleanup code doesn't zap it! */
			req.priv_key = NULL;
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);
}