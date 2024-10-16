static int parseSearchOptions (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	BerElement *ber;
	ber_int_t search_flags;
	ber_tag_t tag;

	if ( BER_BVISNULL( &ctrl->ldctl_value )) {
		rs->sr_text = "searchOptions control value is absent";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISEMPTY( &ctrl->ldctl_value )) {
		rs->sr_text = "searchOptions control value is empty";
		return LDAP_PROTOCOL_ERROR;
	}

	ber = ber_init( &ctrl->ldctl_value );
	if( ber == NULL ) {
		rs->sr_text = "internal error";
		return LDAP_OTHER;
	}

	tag = ber_scanf( ber, "{i}", &search_flags );
	(void) ber_free( ber, 1 );

	if ( tag == LBER_ERROR ) {
		rs->sr_text = "searchOptions control decoding error";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( search_flags & ~(LDAP_SEARCH_FLAG_DOMAIN_SCOPE) ) {
		/* Search flags not recognised so far,
		 * including:
		 *		LDAP_SEARCH_FLAG_PHANTOM_ROOT
		 */
		if ( ctrl->ldctl_iscritical ) {
			rs->sr_text = "searchOptions contained unrecognized flag";
			return LDAP_UNWILLING_TO_PERFORM;
		}

		/* Ignore */
		Debug( LDAP_DEBUG_TRACE,
			"searchOptions: conn=%lu unrecognized flag(s) 0x%x (non-critical)\n", 
			op->o_connid, (unsigned)search_flags, 0 );

		return LDAP_SUCCESS;
	}

	if ( search_flags & LDAP_SEARCH_FLAG_DOMAIN_SCOPE ) {
		if ( op->o_domain_scope != SLAP_CONTROL_NONE ) {
			rs->sr_text = "searchOptions control specified multiple times "
				"or with domainScope control";
			return LDAP_PROTOCOL_ERROR;
		}

		op->o_domain_scope = ctrl->ldctl_iscritical
			? SLAP_CONTROL_CRITICAL
			: SLAP_CONTROL_NONCRITICAL;
	}

	return LDAP_SUCCESS;
}