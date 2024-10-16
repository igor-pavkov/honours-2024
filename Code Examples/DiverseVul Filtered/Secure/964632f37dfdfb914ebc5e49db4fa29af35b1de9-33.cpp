int gnutls_ocsp_req_get_version(gnutls_ocsp_req_t req)
{
	uint8_t version[8];
	int len, ret;

	if (req == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	len = sizeof(version);
	ret =
	    asn1_read_value(req->req, "tbsRequest.version", version, &len);
	if (ret != ASN1_SUCCESS) {
		if (ret == ASN1_ELEMENT_NOT_FOUND)
			return 1;	/* the DEFAULT version */
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	return (int) version[0] + 1;
}