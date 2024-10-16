static int check_cert(X509_STORE *ctx, X509 *x, STACK_OF(X509) *untrustedchain, int purpose)
{
	int ret=0;
	X509_STORE_CTX *csc;
	TSRMLS_FETCH();

	csc = X509_STORE_CTX_new();
	if (csc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "memory allocation failure");
		return 0;
	}
	X509_STORE_CTX_init(csc, ctx, x, untrustedchain);
	if(purpose >= 0) {
		X509_STORE_CTX_set_purpose(csc, purpose);
	}
	ret = X509_verify_cert(csc);
	X509_STORE_CTX_free(csc);

	return ret;
}