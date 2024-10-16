char* r_pkcs7_signerinfos_dump (RX509CertificateRevocationList *crl, char* buffer, ut32 length, const char* pad) {
	RASN1String *algo = NULL, *last = NULL, *next = NULL;
	ut32 i, p;
	int r;
	char *tmp, *pad2, *pad3;
	if (!crl || !buffer || !length) {
		return NULL;
	}
	if (!pad) {
		pad = "";
	}
	pad3 = r_str_newf ("%s    ", pad);
	if (!pad3) return NULL;

	pad2 = pad3 + 2;
	algo = crl->signature.algorithm;
	last = crl->lastUpdate;
	next = crl->nextUpdate;
	r = snprintf (buffer, length, "%sCRL:\n%sSignature:\n%s%s\n%sIssuer\n",
				pad, pad2, pad3, algo ? algo->string : "", pad2);
	p = (ut32) r;
	if (r < 0 || !(tmp = r_x509_name_dump (&crl->issuer, buffer + p, length - p, pad3))) {
		free (pad3);
		return NULL;
	}
	p = tmp - buffer;
	if (length <= p) {
		free (pad3);
		return NULL;
	}
	r = snprintf (buffer + p, length - p, "%sLast Update: %s\n%sNext Update: %s\n%sRevoked Certificates:\n",
				pad2, last ? last->string : "Missing",
				pad2, next ? next->string : "Missing", pad2);
	p += (ut32) r;
	if (r < 0) {
		free (pad3);
		return NULL;
	}
	for (i = 0; i < crl->length; ++i) {
		if (length <= p || !(tmp = r_x509_crlentry_dump (crl->revokedCertificates[i], buffer + p, length - p, pad3))) {
			free (pad3);
			return NULL;
		}
		p = tmp - buffer;
	}

	free (pad3);
	return buffer + p;
}