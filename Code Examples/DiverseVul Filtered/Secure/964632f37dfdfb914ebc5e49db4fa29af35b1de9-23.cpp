int gnutls_ocsp_resp_get_signature_algorithm(gnutls_ocsp_resp_t resp)
{
	int ret;
	gnutls_datum_t sa;

	ret = _gnutls_x509_read_value(resp->basicresp,
				      "signatureAlgorithm.algorithm", &sa);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret = gnutls_oid_to_sign((char *) sa.data);

	_gnutls_free_datum(&sa);

	return ret;
}