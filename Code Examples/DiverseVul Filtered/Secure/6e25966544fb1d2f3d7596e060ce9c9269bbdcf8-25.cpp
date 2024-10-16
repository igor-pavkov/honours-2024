static int netsnmp_session_gen_auth_key(struct snmp_session *s, char *pass)
{
	int snmp_errno;
	s->securityAuthKeyLen = USM_AUTH_KU_LEN;
	if ((snmp_errno = generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
			(u_char *) pass, strlen(pass),
			s->securityAuthKey, &(s->securityAuthKeyLen)))) {
		php_error_docref(NULL, E_WARNING, "Error generating a key for authentication pass phrase '%s': %s", pass, snmp_api_errstring(snmp_errno));
		return (-1);
	}
	return (0);
}