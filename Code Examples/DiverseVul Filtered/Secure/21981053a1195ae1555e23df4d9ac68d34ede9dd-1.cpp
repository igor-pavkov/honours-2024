static int parsePermissiveModify (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	if ( op->o_permissive_modify != SLAP_CONTROL_NONE ) {
		rs->sr_text = "permissiveModify control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( !BER_BVISNULL( &ctrl->ldctl_value )) {
		rs->sr_text = "permissiveModify control value not absent";
		return LDAP_PROTOCOL_ERROR;
	}

	op->o_permissive_modify = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	return LDAP_SUCCESS;
}