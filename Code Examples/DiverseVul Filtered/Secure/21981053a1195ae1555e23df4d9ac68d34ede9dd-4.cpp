static int parseProxyAuthz (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	int		rc;
	struct berval	dn = BER_BVNULL;

	if ( op->o_proxy_authz != SLAP_CONTROL_NONE ) {
		rs->sr_text = "proxy authorization control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISNULL( &ctrl->ldctl_value )) {
		rs->sr_text = "proxy authorization control value absent";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( ( global_disallows & SLAP_DISALLOW_PROXY_AUTHZ_N_CRIT )
		&& !ctrl->ldctl_iscritical )
	{
		rs->sr_text = "proxied authorization criticality of FALSE not allowed";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( !( global_allows & SLAP_ALLOW_PROXY_AUTHZ_ANON )
		&& BER_BVISEMPTY( &op->o_ndn ) )
	{
		rs->sr_text = "anonymous proxied authorization not allowed";
		return LDAP_PROXIED_AUTHORIZATION_DENIED;
	}

	op->o_proxy_authz = ctrl->ldctl_iscritical
		? SLAP_CONTROL_CRITICAL
		: SLAP_CONTROL_NONCRITICAL;

	Debug( LDAP_DEBUG_ARGS,
		"parseProxyAuthz: conn %lu authzid=\"%s\"\n", 
		op->o_connid,
		ctrl->ldctl_value.bv_len ?  ctrl->ldctl_value.bv_val : "anonymous",
		0 );

	if ( BER_BVISEMPTY( &ctrl->ldctl_value )) {
		Debug( LDAP_DEBUG_TRACE,
			"parseProxyAuthz: conn=%lu anonymous\n", 
			op->o_connid, 0, 0 );

		/* anonymous */
		if ( !BER_BVISNULL( &op->o_ndn ) ) {
			op->o_ndn.bv_val[ 0 ] = '\0';
		}
		op->o_ndn.bv_len = 0;

		if ( !BER_BVISNULL( &op->o_dn ) ) {
			op->o_dn.bv_val[ 0 ] = '\0';
		}
		op->o_dn.bv_len = 0;

		return LDAP_SUCCESS;
	}

	rc = slap_sasl_getdn( op->o_conn, op, &ctrl->ldctl_value,
			NULL, &dn, SLAP_GETDN_AUTHZID );

	/* FIXME: empty DN in proxyAuthz control should be legal... */
	if( rc != LDAP_SUCCESS /* || !dn.bv_len */ ) {
		if ( dn.bv_val ) {
			ch_free( dn.bv_val );
		}
		rs->sr_text = "authzId mapping failed";
		return LDAP_PROXIED_AUTHORIZATION_DENIED;
	}

	Debug( LDAP_DEBUG_TRACE,
		"parseProxyAuthz: conn=%lu \"%s\"\n", 
		op->o_connid,
		dn.bv_len ? dn.bv_val : "(NULL)", 0 );

	rc = slap_sasl_authorized( op, &op->o_ndn, &dn );

	if ( rc ) {
		ch_free( dn.bv_val );
		rs->sr_text = "not authorized to assume identity";
		return LDAP_PROXIED_AUTHORIZATION_DENIED;
	}

	ch_free( op->o_ndn.bv_val );

	/*
	 * NOTE: since slap_sasl_getdn() returns a normalized dn,
	 * from now on op->o_dn is normalized
	 */
	op->o_ndn = dn;
	ber_bvreplace( &op->o_dn, &dn );

	Statslog( LDAP_DEBUG_STATS, "%s PROXYAUTHZ dn=\"%s\"\n",
	    op->o_log_prefix, dn.bv_val, 0, 0, 0 );

	return LDAP_SUCCESS;
}