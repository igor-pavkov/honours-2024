int php_openssl_apply_verification_policy(SSL *ssl, X509 *peer, php_stream *stream TSRMLS_DC) /* {{{ */
{
	zval **val = NULL;
	char *cnmatch = NULL;
	X509_NAME *name;
	char buf[1024];
	int err;

	/* verification is turned off */
	if (!(GET_VER_OPT("verify_peer") && zval_is_true(*val))) {
		return SUCCESS;
	}

	if (peer == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not get peer certificate");
		return FAILURE;
	}

	err = SSL_get_verify_result(ssl);
	switch (err) {
		case X509_V_OK:
			/* fine */
			break;
		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
			if (GET_VER_OPT("allow_self_signed") && zval_is_true(*val)) {
				/* allowed */
				break;
			}
			/* not allowed, so fall through */
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not verify peer: code:%d %s", err, X509_verify_cert_error_string(err));
			return FAILURE;
	}

	/* if the cert passed the usual checks, apply our own local policies now */

	name = X509_get_subject_name(peer);

	/* Does the common name match ? (used primarily for https://) */
	GET_VER_OPT_STRING("CN_match", cnmatch);
	if (cnmatch) {
		int match = 0;
		int name_len = X509_NAME_get_text_by_NID(name, NID_commonName, buf, sizeof(buf));

		if (name_len == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate peer certificate CN");
			return FAILURE;
		} else if (name_len != strlen(buf)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Peer certificate CN=`%.*s' is malformed", name_len, buf);
			return FAILURE;
		}

		match = strcasecmp(cnmatch, buf) == 0;
		if (!match && strlen(buf) > 3 && buf[0] == '*' && buf[1] == '.') {
			/* Try wildcard */

			if (strchr(buf+2, '.')) {
				char *tmp = strstr(cnmatch, buf+1);

				match = tmp && strcasecmp(tmp, buf+2) && tmp == strchr(cnmatch, '.');
			}
		}

		if (!match) {
			/* didn't match */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Peer certificate CN=`%.*s' did not match expected CN=`%s'", name_len, buf, cnmatch);
			return FAILURE;
		}
	}

	return SUCCESS;
}