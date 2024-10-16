static gnutls_x509_crt_t find_signercert(gnutls_ocsp_resp_t resp)
{
	int rc;
	gnutls_x509_crt_t *certs = NULL;
	size_t ncerts = 0, i;
	gnutls_datum_t riddn = {NULL, 0};
	gnutls_datum_t keyid = {NULL, 0};
	gnutls_x509_crt_t signercert = NULL;

	rc = gnutls_ocsp_resp_get_responder_raw_id(resp, GNUTLS_OCSP_RESP_ID_DN, &riddn);
	if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
		gnutls_assert();
		rc = gnutls_ocsp_resp_get_responder_raw_id(resp, GNUTLS_OCSP_RESP_ID_KEY, &keyid);
	}
	if (rc != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return NULL;
	}

	rc = gnutls_ocsp_resp_get_certs(resp, &certs, &ncerts);
	if (rc != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		signercert = NULL;
		goto quit;
	}

	for (i = 0; i < ncerts; i++) {
		if (keyid.data != NULL) {
			uint8_t digest[20];
			gnutls_datum_t spki;

			rc = _gnutls_x509_get_raw_field2(certs[i]->cert, &certs[i]->der,
					  "tbsCertificate.subjectPublicKeyInfo.subjectPublicKey",
					  &spki);
			if (rc < 0 || spki.size < 6) {
				signercert = NULL;
				goto quit;
			}

			/* For some reason the protocol requires we skip the
			 * tag, length and number of unused bits.
			 */
			spki.data += 5;
			spki.size -= 5;
			rc = gnutls_hash_fast(GNUTLS_DIG_SHA1, spki.data, spki.size, digest);
			if (rc < 0) {
				gnutls_assert();
				signercert = NULL;
				goto quit;
			}

			if ((20 == keyid.size) &&
				memcmp(keyid.data, digest, 20) == 0) {
				signercert = certs[i];
				goto quit;
			}
		} else {
			if ((certs[i]->raw_dn.size == riddn.size)
			    && memcmp(riddn.data, certs[i]->raw_dn.data, riddn.size) == 0) {
				signercert = certs[i];
				goto quit;
			}
		}
	}

	gnutls_assert();
	signercert = NULL;

      quit:
	gnutls_free(riddn.data);
	gnutls_free(keyid.data);
	for (i = 0; i < ncerts; i++)
		if (certs[i] != signercert)
			gnutls_x509_crt_deinit(certs[i]);
	gnutls_free(certs);
	return signercert;
}