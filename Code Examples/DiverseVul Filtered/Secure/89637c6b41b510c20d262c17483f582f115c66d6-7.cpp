static EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req TSRMLS_DC)
{
	char * randfile = NULL;
	int egdsocket, seeded;
	EVP_PKEY * return_val = NULL;
	
	if (req->priv_key_bits < MIN_KEY_LENGTH) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key length is too short; it needs to be at least %d bits, not %d",
				MIN_KEY_LENGTH, req->priv_key_bits);
		return NULL;
	}

	randfile = CONF_get_string(req->req_config, req->section_name, "RANDFILE");
	php_openssl_load_rand_file(randfile, &egdsocket, &seeded TSRMLS_CC);
	
	if ((req->priv_key = EVP_PKEY_new()) != NULL) {
		switch(req->priv_key_type) {
			case OPENSSL_KEYTYPE_RSA:
				PHP_OPENSSL_RAND_ADD_TIME();
				if (EVP_PKEY_assign_RSA(req->priv_key, RSA_generate_key(req->priv_key_bits, 0x10001, NULL, NULL))) {
					return_val = req->priv_key;
				}
				break;
#if !defined(NO_DSA) && defined(HAVE_DSA_DEFAULT_METHOD)
			case OPENSSL_KEYTYPE_DSA:
				PHP_OPENSSL_RAND_ADD_TIME();
				{
					DSA *dsapar = DSA_generate_parameters(req->priv_key_bits, NULL, 0, NULL, NULL, NULL, NULL);
					if (dsapar) {
						DSA_set_method(dsapar, DSA_get_default_method());
						if (DSA_generate_key(dsapar)) {
							if (EVP_PKEY_assign_DSA(req->priv_key, dsapar)) {
								return_val = req->priv_key;
							}
						} else {
							DSA_free(dsapar);
						}
					}
				}
				break;
#endif
#if !defined(NO_DH)
			case OPENSSL_KEYTYPE_DH:
				PHP_OPENSSL_RAND_ADD_TIME();
				{
					DH *dhpar = DH_generate_parameters(req->priv_key_bits, 2, NULL, NULL);
					int codes = 0;

					if (dhpar) {
						DH_set_method(dhpar, DH_get_default_method());
						if (DH_check(dhpar, &codes) && codes == 0 && DH_generate_key(dhpar)) {
							if (EVP_PKEY_assign_DH(req->priv_key, dhpar)) {
								return_val = req->priv_key;
							}
						} else {
							DH_free(dhpar);
						}
					}
				}
				break;
#endif
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported private key type");
		}
	}

	php_openssl_write_rand_file(randfile, egdsocket, seeded);
	
	if (return_val == NULL) {
		EVP_PKEY_free(req->priv_key);
		req->priv_key = NULL;
		return NULL;
	}
	
	return return_val;
}