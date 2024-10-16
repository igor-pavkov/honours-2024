static int parseAssert (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	BerElement	*ber;
	struct berval	fstr = BER_BVNULL;

	if ( op->o_assert != SLAP_CONTROL_NONE ) {
		rs->sr_text = "assert control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISNULL( &ctrl->ldctl_value )) {
		rs->sr_text = "assert control value is absent";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISEMPTY( &ctrl->ldctl_value )) {
		rs->sr_text = "assert control value is empty";
		return LDAP_PROTOCOL_ERROR;
	}

	ber = ber_init( &(ctrl->ldctl_value) );
	if (ber == NULL) {
		rs->sr_text = "assert control: internal error";
		return LDAP_OTHER;
	}

	rs->sr_err = get_filter( op, ber, (Filter **)&(op->o_assertion),
		&rs->sr_text);
	(void) ber_free( ber, 1 );
	if( rs->sr_err != LDAP_SUCCESS ) {
		if( rs->sr_err == SLAPD_DISCONNECT ) {
			rs->sr_err = LDAP_PROTOCOL_ERROR;
			send_ldap_disconnect( op, rs );
			rs->sr_err = SLAPD_DISCONNECT;
		} else {
			send_ldap_result( op, rs );
		}
		if( op->o_assertion != NULL ) {
			filter_free_x( op, op->o_assertion, 1 );
			op->o_assertion = NULL;
		}
		return rs->sr_err;
	}

#ifdef LDAP_DEBUG
	filter2bv_x( op, op->o_assertion, &fstr );

	Debug( LDAP_DEBUG_ARGS, "parseAssert: conn %ld assert: %s\n",
		op->o_connid, fstr.bv_len ? fstr.bv_val : "empty" , 0 );
	op->o_tmpfree( fstr.bv_val, op->o_tmpmemctx );
#endif

	op->o_assert = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	rs->sr_err = LDAP_SUCCESS;
	return LDAP_SUCCESS;
}