static HostnameValidationResult matches_subject_alternative_name(const char *hostname, const X509 *server_cert) {
	HostnameValidationResult result = MatchNotFound;
	int i;
	int san_names_nb = -1;
	STACK_OF(GENERAL_NAME) *san_names = NULL;

	// Try to extract the names within the SAN extension from the certificate
	san_names = X509_get_ext_d2i((X509 *) server_cert, NID_subject_alt_name, NULL, NULL);
	if (san_names == NULL) {
		return NoSANPresent;
	}
	san_names_nb = sk_GENERAL_NAME_num(san_names);

	// Check each name within the extension
	for (i=0; i<san_names_nb; i++) {
		const GENERAL_NAME *current_name = sk_GENERAL_NAME_value(san_names, i);

		if (current_name->type == GEN_DNS) {
			// Current name is a DNS name, let's check it
			char *dns_name = (char *) ASN1_STRING_data(current_name->d.dNSName);

			// Make sure there isn't an embedded NUL character in the DNS name
			if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
				result = MalformedCertificate;
				break;
			}
			else { // Compare expected hostname with the DNS name
				if (strcasecmp(hostname, dns_name) == 0) {
					result = MatchFound;
					break;
				}
			}
		}
	}
	sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

	return result;
}