PHP_MINFO_FUNCTION(ldap)
{
	char tmp[32];
#if HAVE_NSLDAP
	LDAPVersion ver;
	double SDKVersion;
#endif

	php_info_print_table_start();
	php_info_print_table_row(2, "LDAP Support", "enabled");
	php_info_print_table_row(2, "RCS Version", "$Id$");

	if (LDAPG(max_links) == -1) {
		snprintf(tmp, 31, "%ld/unlimited", LDAPG(num_links));
	} else {
		snprintf(tmp, 31, "%ld/%ld", LDAPG(num_links), LDAPG(max_links));
	}
	php_info_print_table_row(2, "Total Links", tmp);

#ifdef LDAP_API_VERSION
	snprintf(tmp, 31, "%d", LDAP_API_VERSION);
	php_info_print_table_row(2, "API Version", tmp);
#endif

#ifdef LDAP_VENDOR_NAME
	php_info_print_table_row(2, "Vendor Name", LDAP_VENDOR_NAME);
#endif

#ifdef LDAP_VENDOR_VERSION
	snprintf(tmp, 31, "%d", LDAP_VENDOR_VERSION);
	php_info_print_table_row(2, "Vendor Version", tmp);
#endif

#if HAVE_NSLDAP
	SDKVersion = ldap_version(&ver);
	snprintf(tmp, 31, "%F", SDKVersion/100.0);
	php_info_print_table_row(2, "SDK Version", tmp);

	snprintf(tmp, 31, "%F", ver.protocol_version/100.0);
	php_info_print_table_row(2, "Highest LDAP Protocol Supported", tmp);

	snprintf(tmp, 31, "%F", ver.SSL_version/100.0);
	php_info_print_table_row(2, "SSL Level Supported", tmp);

	if (ver.security_level != LDAP_SECURITY_NONE) {
		snprintf(tmp, 31, "%d", ver.security_level);
	} else {
		strcpy(tmp, "SSL not enabled");
	}
	php_info_print_table_row(2, "Level of Encryption", tmp);
#endif

#ifdef HAVE_LDAP_SASL
	php_info_print_table_row(2, "SASL Support", "Enabled");
#endif

	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}