static int parsePagedResults (
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	BerElementBuffer berbuf;
	BerElement	*ber = (BerElement *)&berbuf;
	struct berval	cookie;
	PagedResultsState	*ps;
	int		rc = LDAP_SUCCESS;
	ber_tag_t	tag;
	ber_int_t	size;

	if ( op->o_pagedresults != SLAP_CONTROL_NONE ) {
		rs->sr_text = "paged results control specified multiple times";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISNULL( &ctrl->ldctl_value ) ) {
		rs->sr_text = "paged results control value is absent";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISEMPTY( &ctrl->ldctl_value ) ) {
		rs->sr_text = "paged results control value is empty";
		return LDAP_PROTOCOL_ERROR;
	}

	/* Parse the control value
	 *	realSearchControlValue ::= SEQUENCE {
	 *		size	INTEGER (0..maxInt),
	 *				-- requested page size from client
	 *				-- result set size estimate from server
	 *		cookie	OCTET STRING
	 * }
	 */
	ber_init2( ber, &ctrl->ldctl_value, LBER_USE_DER );

	tag = ber_scanf( ber, "{im}", &size, &cookie );

	if ( tag == LBER_ERROR ) {
		rs->sr_text = "paged results control could not be decoded";
		rc = LDAP_PROTOCOL_ERROR;
		goto done;
	}

	if ( size < 0 ) {
		rs->sr_text = "paged results control size invalid";
		rc = LDAP_PROTOCOL_ERROR;
		goto done;
	}

	ps = op->o_tmpalloc( sizeof(PagedResultsState), op->o_tmpmemctx );
	*ps = op->o_conn->c_pagedresults_state;
	ps->ps_size = size;
	ps->ps_cookieval = cookie;
	op->o_pagedresults_state = ps;
	if ( !cookie.bv_len ) {
		ps->ps_count = 0;
		ps->ps_cookie = 0;
		/* taint ps_cookie, to detect whether it's set */
		op->o_conn->c_pagedresults_state.ps_cookie = NOID;
	}

	/* NOTE: according to RFC 2696 3.:

    If the page size is greater than or equal to the sizeLimit value, the
    server should ignore the control as the request can be satisfied in a
    single page.

	 * NOTE: this assumes that the op->ors_slimit be set
	 * before the controls are parsed.     
	 */

	if ( op->ors_slimit > 0 && size >= op->ors_slimit ) {
		op->o_pagedresults = SLAP_CONTROL_IGNORED;

	} else if ( ctrl->ldctl_iscritical ) {
		op->o_pagedresults = SLAP_CONTROL_CRITICAL;

	} else {
		op->o_pagedresults = SLAP_CONTROL_NONCRITICAL;
	}

done:;
	return rc;
}