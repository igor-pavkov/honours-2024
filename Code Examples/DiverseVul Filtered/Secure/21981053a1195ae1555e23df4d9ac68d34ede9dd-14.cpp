static int parseNoOp (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	if ( op->o_noop != SLAP_CONTROL_NONE ) {
		rs->sr_text = "noop control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( !BER_BVISNULL( &ctrl->ldctl_value ) ) {
		rs->sr_text = "noop control value not empty";
		return LDAP_PROTOCOL_ERROR;
	}

	op->o_noop = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	return LDAP_SUCCESS;
}