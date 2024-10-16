static int check_ocsp_purpose(gnutls_x509_crt_t signercert)
{
	char oidtmp[MAX_OID_SIZE];
	size_t oidsize;
	int indx, rc;

	for (indx = 0;; indx++) {
		oidsize = sizeof(oidtmp);
		rc = gnutls_x509_crt_get_key_purpose_oid(signercert, indx,
							 oidtmp, &oidsize,
							 NULL);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
			gnutls_assert();
			return rc;
		} else if (rc == GNUTLS_E_SHORT_MEMORY_BUFFER) {
			gnutls_assert();
			continue;
		} else if (rc != GNUTLS_E_SUCCESS) {
			return gnutls_assert_val(rc);
		}

		if (memcmp(oidtmp, GNUTLS_KP_OCSP_SIGNING, oidsize) != 0) {
			gnutls_assert();
			continue;
		}
		break;
	}

	return 0;
}