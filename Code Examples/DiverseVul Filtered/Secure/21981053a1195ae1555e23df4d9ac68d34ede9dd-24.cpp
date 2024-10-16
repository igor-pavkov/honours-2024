int get_ctrls(
	Operation *op,
	SlapReply *rs,
	int sendres )
{
	int nctrls = 0;
	ber_tag_t tag;
	ber_len_t len;
	char *opaque;
	BerElement *ber = op->o_ber;
	struct berval bv;
#ifdef SLAP_CONTROL_X_WHATFAILED
	/* NOTE: right now, slapd checks the validity of each control
	 * while parsing.  As a consequence, it can only detect one
	 * cause of failure at a time.  This results in returning
	 * exactly one OID with the whatFailed control, or no control
	 * at all.
	 */
	char *failed_oid = NULL;
#endif

	len = ber_pvt_ber_remaining(ber);

	if( len == 0) {
		/* no controls */
		rs->sr_err = LDAP_SUCCESS;
		return rs->sr_err;
	}

	if(( tag = ber_peek_tag( ber, &len )) != LDAP_TAG_CONTROLS ) {
		if( tag == LBER_ERROR ) {
			rs->sr_err = SLAPD_DISCONNECT;
			rs->sr_text = "unexpected data in PDU";
		}

		goto return_results;
	}

	Debug( LDAP_DEBUG_TRACE,
		"=> get_ctrls\n", 0, 0, 0 );

	if( op->o_protocol < LDAP_VERSION3 ) {
		rs->sr_err = SLAPD_DISCONNECT;
		rs->sr_text = "controls require LDAPv3";
		goto return_results;
	}

	/* one for first control, one for termination */
	op->o_ctrls = op->o_tmpalloc( 2 * sizeof(LDAPControl *), op->o_tmpmemctx );

#if 0
	if( op->ctrls == NULL ) {
		rs->sr_err = LDAP_NO_MEMORY;
		rs->sr_text = "no memory";
		goto return_results;
	}
#endif

	op->o_ctrls[nctrls] = NULL;

	/* step through each element */
	for( tag = ber_first_element( ber, &len, &opaque );
		tag != LBER_ERROR;
		tag = ber_next_element( ber, &len, opaque ) )
	{
		LDAPControl *c;
		LDAPControl **tctrls;

		c = op->o_tmpalloc( sizeof(LDAPControl), op->o_tmpmemctx );
		memset(c, 0, sizeof(LDAPControl));

		/* allocate pointer space for current controls (nctrls)
		 * + this control + extra NULL
		 */
		tctrls = op->o_tmprealloc( op->o_ctrls,
			(nctrls+2) * sizeof(LDAPControl *), op->o_tmpmemctx );

#if 0
		if( tctrls == NULL ) {
			ch_free( c );
			ldap_controls_free(op->o_ctrls);
			op->o_ctrls = NULL;

			rs->sr_err = LDAP_NO_MEMORY;
			rs->sr_text = "no memory";
			goto return_results;
		}
#endif
		op->o_ctrls = tctrls;

		op->o_ctrls[nctrls++] = c;
		op->o_ctrls[nctrls] = NULL;

		tag = ber_scanf( ber, "{m" /*}*/, &bv );
		c->ldctl_oid = bv.bv_val;

		if( tag == LBER_ERROR ) {
			Debug( LDAP_DEBUG_TRACE, "=> get_ctrls: get oid failed.\n",
				0, 0, 0 );

			slap_free_ctrls( op, op->o_ctrls );
			op->o_ctrls = NULL;
			rs->sr_err = SLAPD_DISCONNECT;
			rs->sr_text = "decoding controls error";
			goto return_results;

		} else if( c->ldctl_oid == NULL ) {
			Debug( LDAP_DEBUG_TRACE,
				"get_ctrls: conn %lu got empty OID.\n",
				op->o_connid, 0, 0 );

			slap_free_ctrls( op, op->o_ctrls );
			op->o_ctrls = NULL;
			rs->sr_err = LDAP_PROTOCOL_ERROR;
			rs->sr_text = "OID field is empty";
			goto return_results;
		}

		tag = ber_peek_tag( ber, &len );

		if( tag == LBER_BOOLEAN ) {
			ber_int_t crit;
			tag = ber_scanf( ber, "b", &crit );

			if( tag == LBER_ERROR ) {
				Debug( LDAP_DEBUG_TRACE, "=> get_ctrls: get crit failed.\n",
					0, 0, 0 );
				slap_free_ctrls( op, op->o_ctrls );
				op->o_ctrls = NULL;
				rs->sr_err = SLAPD_DISCONNECT;
				rs->sr_text = "decoding controls error";
				goto return_results;
			}

			c->ldctl_iscritical = (crit != 0);
			tag = ber_peek_tag( ber, &len );
		}

		if( tag == LBER_OCTETSTRING ) {
			tag = ber_scanf( ber, "m", &c->ldctl_value );

			if( tag == LBER_ERROR ) {
				Debug( LDAP_DEBUG_TRACE, "=> get_ctrls: conn %lu: "
					"%s (%scritical): get value failed.\n",
					op->o_connid, c->ldctl_oid,
					c->ldctl_iscritical ? "" : "non" );
				slap_free_ctrls( op, op->o_ctrls );
				op->o_ctrls = NULL;
				rs->sr_err = SLAPD_DISCONNECT;
				rs->sr_text = "decoding controls error";
				goto return_results;
			}
		}

		Debug( LDAP_DEBUG_TRACE,
			"=> get_ctrls: oid=\"%s\" (%scritical)\n",
			c->ldctl_oid, c->ldctl_iscritical ? "" : "non", 0 );

		rs->sr_err = slap_parse_ctrl( op, rs, c, &rs->sr_text );
		if ( rs->sr_err != LDAP_SUCCESS ) {
#ifdef SLAP_CONTROL_X_WHATFAILED
			failed_oid = c->ldctl_oid;
#endif
			goto return_results;
		}
	}

return_results:
	Debug( LDAP_DEBUG_TRACE,
		"<= get_ctrls: n=%d rc=%d err=\"%s\"\n",
		nctrls, rs->sr_err, rs->sr_text ? rs->sr_text : "");

	if( sendres && rs->sr_err != LDAP_SUCCESS ) {
		if( rs->sr_err == SLAPD_DISCONNECT ) {
			rs->sr_err = LDAP_PROTOCOL_ERROR;
			send_ldap_disconnect( op, rs );
			rs->sr_err = SLAPD_DISCONNECT;
		} else {
#ifdef SLAP_CONTROL_X_WHATFAILED
			/* might have not been parsed yet? */
			if ( failed_oid != NULL ) {
				if ( !get_whatFailed( op ) ) {
					/* look it up */

					/* step through each remaining element */
					for ( ; tag != LBER_ERROR; tag = ber_next_element( ber, &len, opaque ) )
					{
						LDAPControl c = { 0 };

						tag = ber_scanf( ber, "{m" /*}*/, &bv );
						c.ldctl_oid = bv.bv_val;

						if ( tag == LBER_ERROR ) {
							slap_free_ctrls( op, op->o_ctrls );
							op->o_ctrls = NULL;
							break;

						} else if ( c.ldctl_oid == NULL ) {
							slap_free_ctrls( op, op->o_ctrls );
							op->o_ctrls = NULL;
							break;
						}

						tag = ber_peek_tag( ber, &len );
						if ( tag == LBER_BOOLEAN ) {
							ber_int_t crit;
							tag = ber_scanf( ber, "b", &crit );
							if( tag == LBER_ERROR ) {
								slap_free_ctrls( op, op->o_ctrls );
								op->o_ctrls = NULL;
								break;
							}

							tag = ber_peek_tag( ber, &len );
						}

						if ( tag == LBER_OCTETSTRING ) {
							tag = ber_scanf( ber, "m", &c.ldctl_value );

							if( tag == LBER_ERROR ) {
								slap_free_ctrls( op, op->o_ctrls );
								op->o_ctrls = NULL;
								break;
							}
						}

						if ( strcmp( c.ldctl_oid, LDAP_CONTROL_X_WHATFAILED ) == 0 ) {
							const char *text;
							slap_parse_ctrl( op, rs, &c, &text );
							break;
						}
					}
				}

				if ( get_whatFailed( op ) ) {
					char *oids[ 2 ];
					oids[ 0 ] = failed_oid;
					oids[ 1 ] = NULL;
					slap_ctrl_whatFailed_add( op, rs, oids );
				}
			}
#endif

			send_ldap_result( op, rs );
		}
	}

	return rs->sr_err;
}