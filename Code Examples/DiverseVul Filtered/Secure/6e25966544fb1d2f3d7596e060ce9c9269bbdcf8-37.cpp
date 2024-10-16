static int netsnmp_session_set_contextEngineID(struct snmp_session *s, char * contextEngineID)
{
	size_t	ebuf_len = 32, eout_len = 0;
	u_char	*ebuf = (u_char *) emalloc(ebuf_len);

	if (ebuf == NULL) {
		php_error_docref(NULL, E_WARNING, "malloc failure setting contextEngineID");
		return (-1);
	}
	if (!snmp_hex_to_binary(&ebuf, &ebuf_len, &eout_len, 1, contextEngineID)) {
		php_error_docref(NULL, E_WARNING, "Bad engine ID value '%s'", contextEngineID);
		efree(ebuf);
		return (-1);
	}

	if (s->contextEngineID) {
		efree(s->contextEngineID);
	}

	s->contextEngineID = ebuf;
	s->contextEngineIDLen = eout_len;
	return (0);
}