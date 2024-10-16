static int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot)
{
	if (!strcasecmp(prot, "DES")) {
		s->securityPrivProto = usmDESPrivProtocol;
		s->securityPrivProtoLen = USM_PRIV_PROTO_DES_LEN;
#ifdef HAVE_AES
	} else if (!strcasecmp(prot, "AES128") || !strcasecmp(prot, "AES")) {
		s->securityPrivProto = usmAESPrivProtocol;
		s->securityPrivProtoLen = USM_PRIV_PROTO_AES_LEN;
#endif
	} else {
		php_error_docref(NULL, E_WARNING, "Unknown security protocol '%s'", prot);
		return (-1);
	}
	return (0);
}