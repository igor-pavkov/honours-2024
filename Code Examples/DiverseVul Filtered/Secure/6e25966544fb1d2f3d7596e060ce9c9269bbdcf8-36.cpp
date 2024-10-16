static void netsnmp_session_free(php_snmp_session **session) /* {{{ */
{
	if (*session) {
		PHP_SNMP_SESSION_FREE(peername);
		PHP_SNMP_SESSION_FREE(community);
		PHP_SNMP_SESSION_FREE(securityName);
		PHP_SNMP_SESSION_FREE(contextEngineID);
		efree(*session);
		*session = NULL;
	}
}