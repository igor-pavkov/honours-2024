int gnutls_ocsp_resp_init(gnutls_ocsp_resp_t * resp)
{
	gnutls_ocsp_resp_t tmp =
	    gnutls_calloc(1, sizeof(gnutls_ocsp_resp_int));
	int ret;

	if (!tmp)
		return GNUTLS_E_MEMORY_ERROR;

	ret = asn1_create_element(_gnutls_get_pkix(),
				  "PKIX1.OCSPResponse", &tmp->resp);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		gnutls_free(tmp);
		return _gnutls_asn2err(ret);
	}

	ret = asn1_create_element(_gnutls_get_pkix(),
				  "PKIX1.BasicOCSPResponse",
				  &tmp->basicresp);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		asn1_delete_structure(&tmp->resp);
		gnutls_free(tmp);
		return _gnutls_asn2err(ret);
	}

	*resp = tmp;

	return GNUTLS_E_SUCCESS;
}