void gnutls_ocsp_resp_deinit(gnutls_ocsp_resp_t resp)
{
	if (!resp)
		return;

	if (resp->resp)
		asn1_delete_structure(&resp->resp);
	gnutls_free(resp->response_type_oid.data);
	if (resp->basicresp)
		asn1_delete_structure(&resp->basicresp);

	resp->resp = NULL;
	resp->response_type_oid.data = NULL;
	resp->basicresp = NULL;

	gnutls_free(resp->der.data);
	gnutls_free(resp);
}