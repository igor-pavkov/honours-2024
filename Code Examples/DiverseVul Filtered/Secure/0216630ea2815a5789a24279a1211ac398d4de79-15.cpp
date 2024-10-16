PHP_FUNCTION(openssl_csr_new)
{
	struct php_x509_request req;
	zval * args = NULL, * dn, *attribs = NULL;
	zval * out_pkey;
	X509_REQ * csr = NULL;
	int we_made_the_key = 1;
	zend_resource *key_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "az/|a!a!", &dn, &out_pkey, &args, &attribs) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		/* Generate or use a private key */
		if (Z_TYPE_P(out_pkey) != IS_NULL) {
			req.priv_key = php_openssl_evp_from_zval(out_pkey, 0, NULL, 0, 0, &key_resource);
			if (req.priv_key != NULL) {
				we_made_the_key = 0;
			}
		}
		if (req.priv_key == NULL) {
			php_openssl_generate_private_key(&req);
		}
		if (req.priv_key == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to generate a private key");
		} else {
			csr = X509_REQ_new();
			if (csr) {
				if (php_openssl_make_REQ(&req, csr, dn, attribs) == SUCCESS) {
					X509V3_CTX ext_ctx;

					X509V3_set_ctx(&ext_ctx, NULL, NULL, csr, NULL, 0);
					X509V3_set_conf_lhash(&ext_ctx, req.req_config);

					/* Add extensions */
					if (req.request_extensions_section && !X509V3_EXT_REQ_add_conf(req.req_config,
								&ext_ctx, req.request_extensions_section, csr))
					{
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING, "Error loading extension section %s", req.request_extensions_section);
					} else {
						RETVAL_TRUE;

						if (X509_REQ_sign(csr, req.priv_key, req.digest)) {
							ZVAL_RES(return_value, zend_register_resource(csr, le_csr));
							csr = NULL;
						} else {
							php_openssl_store_errors();
							php_error_docref(NULL, E_WARNING, "Error signing request");
						}

						if (we_made_the_key) {
							/* and a resource for the private key */
							zval_dtor(out_pkey);
							ZVAL_RES(out_pkey, zend_register_resource(req.priv_key, le_key));
							req.priv_key = NULL; /* make sure the cleanup code doesn't zap it! */
						} else if (key_resource != NULL) {
							req.priv_key = NULL; /* make sure the cleanup code doesn't zap it! */
						}
					}
				}
				else {
					if (!we_made_the_key) {
						/* if we have not made the key we are not supposed to zap it by calling dispose! */
						req.priv_key = NULL;
					}
				}
			} else {
				php_openssl_store_errors();
			}

		}
	}
	if (csr) {
		X509_REQ_free(csr);
	}
	PHP_SSL_REQ_DISPOSE(&req);
}