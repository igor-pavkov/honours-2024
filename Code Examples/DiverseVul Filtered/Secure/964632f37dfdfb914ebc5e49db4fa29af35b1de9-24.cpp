time_t gnutls_ocsp_resp_get_produced(gnutls_ocsp_resp_t resp)
{
	char ttime[MAX_TIME];
	int len, ret;
	time_t c_time;

	if (resp == NULL || resp->basicresp == NULL) {
		gnutls_assert();
		return (time_t) (-1);
	}

	len = sizeof(ttime) - 1;
	ret =
	    asn1_read_value(resp->basicresp, "tbsResponseData.producedAt",
			    ttime, &len);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return (time_t) (-1);
	}

	c_time = _gnutls_x509_generalTime2gtime(ttime);

	return c_time;
}