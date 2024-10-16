static int netsnmp_session_set_auth_protocol(struct snmp_session *s, char *prot)
{
	if (!strcasecmp(prot, "MD5")) {
		s->securityAuthProto = usmHMACMD5AuthProtocol;
		s->securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
	} else if (!strcasecmp(prot, "SHA")) {
		s->securityAuthProto = usmHMACSHA1AuthProtocol;
		s->securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
	} else {
		php_error_docref(NULL, E_WARNING, "Unknown authentication protocol '%s'", prot);
		return (-1);
	}
	return (0);
}