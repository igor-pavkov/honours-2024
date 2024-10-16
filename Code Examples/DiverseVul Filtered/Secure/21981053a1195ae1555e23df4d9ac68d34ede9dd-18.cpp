static int parseSubentries (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	if ( op->o_subentries != SLAP_CONTROL_NONE ) {
		rs->sr_text = "subentries control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	/* FIXME: should use BER library */
	if( ( ctrl->ldctl_value.bv_len != 3 )
		|| ( ctrl->ldctl_value.bv_val[0] != 0x01 )
		|| ( ctrl->ldctl_value.bv_val[1] != 0x01 ))
	{
		rs->sr_text = "subentries control value encoding is bogus";
		return LDAP_PROTOCOL_ERROR;
	}

	op->o_subentries = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	if (ctrl->ldctl_value.bv_val[2]) {
		set_subentries_visibility( op );
	}

	return LDAP_SUCCESS;
}