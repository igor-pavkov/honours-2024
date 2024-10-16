HostnameValidationResult validate_hostname(const char *hostname, const X509 *server_cert) {
	HostnameValidationResult result;

	if((hostname == NULL) || (server_cert == NULL))
		return Error;

	// First try the Subject Alternative Names extension
	result = matches_subject_alternative_name(hostname, server_cert);
	if (result == NoSANPresent) {
		// Extension was not found: try the Common Name
		result = matches_common_name(hostname, server_cert);
	}

	return result;
}