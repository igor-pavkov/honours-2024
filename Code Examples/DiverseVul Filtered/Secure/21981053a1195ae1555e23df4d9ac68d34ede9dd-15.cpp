static int parseSortedResults (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	int		rc = LDAP_SUCCESS;

	if ( op->o_sortedresults != SLAP_CONTROL_NONE ) {
		rs->sr_text = "sorted results control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISNULL( &ctrl->ldctl_value ) ) {
		rs->sr_text = "sorted results control value is absent";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISEMPTY( &ctrl->ldctl_value ) ) {
		rs->sr_text = "sorted results control value is empty";
		return LDAP_PROTOCOL_ERROR;
	}

	/* blow off parsing the value */

	op->o_sortedresults = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	return rc;
}