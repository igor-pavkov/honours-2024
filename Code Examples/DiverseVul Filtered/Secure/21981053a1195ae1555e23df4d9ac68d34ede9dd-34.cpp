static int parseDomainScope (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	if ( op->o_domain_scope != SLAP_CONTROL_NONE ) {
		rs->sr_text = "domainScope control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	/* this should be checking BVISNULL, but M$ clients are broken
	 * and include the value even though the M$ spec says it must be
	 * omitted. ITS#9100.
	 */
	if ( !BER_BVISEMPTY( &ctrl->ldctl_value )) {
		rs->sr_text = "domainScope control value not absent";
		return LDAP_PROTOCOL_ERROR;
	}

	op->o_domain_scope = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	return LDAP_SUCCESS;
}