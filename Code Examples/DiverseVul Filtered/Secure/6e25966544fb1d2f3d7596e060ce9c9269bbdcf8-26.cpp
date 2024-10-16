static int netsnmp_session_gen_sec_key(struct snmp_session *s, char *pass)
{
	int snmp_errno;

	s->securityPrivKeyLen = USM_PRIV_KU_LEN;
	if ((snmp_errno = generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
			(u_char *)pass, strlen(pass),
			s->securityPrivKey, &(s->securityPrivKeyLen)))) {
		php_error_docref(NULL, E_WARNING, "Error generating a key for privacy pass phrase '%s': %s", pass, snmp_api_errstring(snmp_errno));
		return (-2);
	}
	return (0);
}