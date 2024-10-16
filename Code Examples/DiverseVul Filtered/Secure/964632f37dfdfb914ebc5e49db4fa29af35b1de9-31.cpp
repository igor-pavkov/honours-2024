int gnutls_ocsp_req_export(gnutls_ocsp_req_t req, gnutls_datum_t * data)
{
	int ret;

	if (req == NULL || data == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	/* XXX remove when we support these fields */
	asn1_write_value(req->req, "tbsRequest.requestorName", NULL, 0);
	asn1_write_value(req->req, "optionalSignature", NULL, 0);

	/* prune extension field if we don't have any extension */
	ret = gnutls_ocsp_req_get_extension(req, 0, NULL, NULL, NULL);
	if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
		asn1_write_value(req->req, "tbsRequest.requestExtensions",
				 NULL, 0);

	return _gnutls_x509_get_raw_field(req->req, "", data);
}