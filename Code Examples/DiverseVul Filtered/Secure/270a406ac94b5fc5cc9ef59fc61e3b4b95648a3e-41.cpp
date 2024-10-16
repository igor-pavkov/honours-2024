static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx) /* {{{ */
{
	php_stream *stream;
	SSL *ssl;
	X509 *err_cert;
	int err, depth, ret;
	zval **val;

	ret = preverify_ok;

	/* determine the status for the current cert */
	err_cert = X509_STORE_CTX_get_current_cert(ctx);
	err = X509_STORE_CTX_get_error(ctx);
	depth = X509_STORE_CTX_get_error_depth(ctx);

	/* conjure the stream & context to use */
	ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
	stream = (php_stream*)SSL_get_ex_data(ssl, ssl_stream_data_index);

	/* if allow_self_signed is set, make sure that verification succeeds */
	if (err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT && GET_VER_OPT("allow_self_signed") && zval_is_true(*val)) {
		ret = 1;
	}

	/* check the depth */
	if (GET_VER_OPT("verify_depth")) {
		convert_to_long_ex(val);

		if (depth > Z_LVAL_PP(val)) {
			ret = 0;
			X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_CHAIN_TOO_LONG);
		}
	}

	return ret;

}