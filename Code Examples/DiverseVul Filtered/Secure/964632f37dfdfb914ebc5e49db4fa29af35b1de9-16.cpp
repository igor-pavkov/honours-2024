int gnutls_ocsp_resp_export(gnutls_ocsp_resp_t resp, gnutls_datum_t * data)
{
	if (resp == NULL || data == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_x509_get_raw_field(resp->resp, "", data);
}