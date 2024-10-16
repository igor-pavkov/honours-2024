static int check_cert(X509_STORE *ctx, X509 *x, STACK_OF(X509) *untrustedchain, int purpose)
{
	int ret=0;
	X509_STORE_CTX *csc;

	csc = X509_STORE_CTX_new();
	if (csc == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_ERROR, "memory allocation failure");
		return 0;
	}
	if (!X509_STORE_CTX_init(csc, ctx, x, untrustedchain)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "cert store initialization failed");
		return 0;
	}
	if (purpose >= 0 && !X509_STORE_CTX_set_purpose(csc, purpose)) {
		php_openssl_store_errors();
	}
	ret = X509_verify_cert(csc);
	if (ret < 0) {
		php_openssl_store_errors();
	}
	X509_STORE_CTX_free(csc);

	return ret;
}