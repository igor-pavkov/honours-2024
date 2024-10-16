int gnutls_ocsp_req_init(gnutls_ocsp_req_t * req)
{
	gnutls_ocsp_req_t tmp =
	    gnutls_calloc(1, sizeof(gnutls_ocsp_req_int));
	int ret;

	if (!tmp)
		return GNUTLS_E_MEMORY_ERROR;

	ret = asn1_create_element(_gnutls_get_pkix(), "PKIX1.OCSPRequest",
				  &tmp->req);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		gnutls_free(tmp);
		return _gnutls_asn2err(ret);
	}

	*req = tmp;

	return GNUTLS_E_SUCCESS;
}