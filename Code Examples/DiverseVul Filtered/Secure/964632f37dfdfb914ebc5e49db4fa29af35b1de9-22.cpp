static inline unsigned int vstatus_to_ocsp_status(unsigned int status)
{
	unsigned int ostatus;

	if (status & GNUTLS_CERT_INSECURE_ALGORITHM)
		ostatus = GNUTLS_OCSP_VERIFY_INSECURE_ALGORITHM;
	else if (status & GNUTLS_CERT_NOT_ACTIVATED)
		ostatus = GNUTLS_OCSP_VERIFY_CERT_NOT_ACTIVATED;
	else if (status & GNUTLS_CERT_EXPIRED)
		ostatus = GNUTLS_OCSP_VERIFY_CERT_EXPIRED;
	else
		ostatus = GNUTLS_OCSP_VERIFY_UNTRUSTED_SIGNER;

	return ostatus;
}