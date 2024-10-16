static STACK_OF(X509) * load_all_certs_from_file(char *certfile)
{
	STACK_OF(X509_INFO) *sk=NULL;
	STACK_OF(X509) *stack=NULL, *ret=NULL;
	BIO *in=NULL;
	X509_INFO *xi;
	TSRMLS_FETCH();

	if(!(stack = sk_X509_new_null())) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "memory allocation failure");
		goto end;
	}

	if (php_openssl_open_base_dir_chk(certfile TSRMLS_CC)) {
		sk_X509_free(stack);
		goto end;
	}

	if(!(in=BIO_new_file(certfile, "r"))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening the file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}

	/* This loads from a file, a stack of x509/crl/pkey sets */
	if(!(sk=PEM_X509_INFO_read_bio(in, NULL, NULL, NULL))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error reading the file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}

	/* scan over it and pull out the certs */
	while (sk_X509_INFO_num(sk)) {
		xi=sk_X509_INFO_shift(sk);
		if (xi->x509 != NULL) {
			sk_X509_push(stack,xi->x509);
			xi->x509=NULL;
		}
		X509_INFO_free(xi);
	}
	if(!sk_X509_num(stack)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "no certificates in file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}
	ret=stack;
end:
	BIO_free(in);
	sk_X509_INFO_free(sk);

	return ret;
}