int gnutls_ocsp_req_randomize_nonce(gnutls_ocsp_req_t req)
{
	int ret;
	uint8_t rndbuf[23];
	gnutls_datum_t nonce = { rndbuf, sizeof(rndbuf) };

	if (req == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = gnutls_rnd(GNUTLS_RND_NONCE, rndbuf, sizeof(rndbuf));
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	ret = gnutls_ocsp_req_set_nonce(req, 0, &nonce);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	return GNUTLS_E_SUCCESS;
}