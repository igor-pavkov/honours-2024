int gnutls_ocsp_resp_get_status(gnutls_ocsp_resp_t resp)
{
	uint8_t str[1];
	int len, ret;

	if (resp == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	len = sizeof(str);
	ret = asn1_read_value(resp->resp, "responseStatus", str, &len);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	switch (str[0]) {
	case GNUTLS_OCSP_RESP_SUCCESSFUL:
	case GNUTLS_OCSP_RESP_MALFORMEDREQUEST:
	case GNUTLS_OCSP_RESP_INTERNALERROR:
	case GNUTLS_OCSP_RESP_TRYLATER:
	case GNUTLS_OCSP_RESP_SIGREQUIRED:
	case GNUTLS_OCSP_RESP_UNAUTHORIZED:
		break;
	default:
		return GNUTLS_E_UNEXPECTED_PACKET;
	}

	return (int) str[0];
}