void gnutls_ocsp_req_deinit(gnutls_ocsp_req_t req)
{
	if (!req)
		return;

	if (req->req)
		asn1_delete_structure(&req->req);

	req->req = NULL;
	gnutls_free(req);
}