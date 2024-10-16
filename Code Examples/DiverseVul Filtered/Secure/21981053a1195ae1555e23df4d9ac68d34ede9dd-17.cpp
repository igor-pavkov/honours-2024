static int parseSessionTracking(
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	BerElement		*ber;
	ber_tag_t		tag;
	ber_len_t		len;
	int			i, rc;

	struct berval		sessionSourceIp = BER_BVNULL,
				sessionSourceName = BER_BVNULL,
				formatOID = BER_BVNULL,
				sessionTrackingIdentifier = BER_BVNULL;

	size_t			st_len, st_pos;

	if ( ctrl->ldctl_iscritical ) {
		rs->sr_text = "sessionTracking criticality is TRUE";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISNULL( &ctrl->ldctl_value ) ) {
		rs->sr_text = "sessionTracking control value is absent";
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISEMPTY( &ctrl->ldctl_value ) ) {
		rs->sr_text = "sessionTracking control value is empty";
		return LDAP_PROTOCOL_ERROR;
	}

	/* TODO: add the capability to determine if a client is allowed
	 * to use this control, based on identity, ip and so */

	ber = ber_init( &ctrl->ldctl_value );
	if ( ber == NULL ) {
		rs->sr_text = "internal error";
		return LDAP_OTHER;
	}

	tag = ber_skip_tag( ber, &len );
	if ( tag != LBER_SEQUENCE ) {
		tag = LBER_ERROR;
		goto error;
	}

	/* sessionSourceIp */
	tag = ber_peek_tag( ber, &len );
	if ( tag == LBER_DEFAULT ) {
		tag = LBER_ERROR;
		goto error;
	}

	if ( len == 0 ) {
		tag = ber_skip_tag( ber, &len );

	} else if ( len > 128 ) {
		rs->sr_text = "sessionTracking.sessionSourceIp too long";
		rs->sr_err = LDAP_PROTOCOL_ERROR;
		goto error;

	} else {
		tag = ber_scanf( ber, "m", &sessionSourceIp );
	}

	if ( ldif_is_not_printable( sessionSourceIp.bv_val, sessionSourceIp.bv_len ) ) {
		BER_BVZERO( &sessionSourceIp );
	}

	/* sessionSourceName */
	tag = ber_peek_tag( ber, &len );
	if ( tag == LBER_DEFAULT ) {
		tag = LBER_ERROR;
		goto error;
	}

	if ( len == 0 ) {
		tag = ber_skip_tag( ber, &len );

	} else if ( len > 65536 ) {
		rs->sr_text = "sessionTracking.sessionSourceName too long";
		rs->sr_err = LDAP_PROTOCOL_ERROR;
		goto error;

	} else {
		tag = ber_scanf( ber, "m", &sessionSourceName );
	}

	if ( ldif_is_not_printable( sessionSourceName.bv_val, sessionSourceName.bv_len ) ) {
		BER_BVZERO( &sessionSourceName );
	}

	/* formatOID */
	tag = ber_peek_tag( ber, &len );
	if ( tag == LBER_DEFAULT ) {
		tag = LBER_ERROR;
		goto error;
	}

	if ( len == 0 ) {
		rs->sr_text = "sessionTracking.formatOID empty";
		rs->sr_err = LDAP_PROTOCOL_ERROR;
		goto error;

	} else if ( len > 1024 ) {
		rs->sr_text = "sessionTracking.formatOID too long";
		rs->sr_err = LDAP_PROTOCOL_ERROR;
		goto error;

	} else {
		tag = ber_scanf( ber, "m", &formatOID );
	}

	rc = numericoidValidate( NULL, &formatOID );
	if ( rc != LDAP_SUCCESS ) {
		rs->sr_text = "sessionTracking.formatOID invalid";
		goto error;
	}

	for ( i = 0; !BER_BVISNULL( &session_tracking_formats[ i ] ); i += 2 )
	{
		if ( bvmatch( &formatOID, &session_tracking_formats[ i ] ) ) {
			formatOID = session_tracking_formats[ i + 1 ];
			break;
		}
	}

	/* sessionTrackingIdentifier */
	tag = ber_peek_tag( ber, &len );
	if ( tag == LBER_DEFAULT ) {
		tag = LBER_ERROR;
		goto error;
	}

	if ( len == 0 ) {
		tag = ber_skip_tag( ber, &len );

	} else {
		/* note: should not be more than 65536... */
		tag = ber_scanf( ber, "m", &sessionTrackingIdentifier );
		if ( ldif_is_not_printable( sessionTrackingIdentifier.bv_val, sessionTrackingIdentifier.bv_len ) ) {
			/* we want the OID printed, at least */
			BER_BVSTR( &sessionTrackingIdentifier, "" );
		}
	}

	/* closure */
	tag = ber_skip_tag( ber, &len );
	if ( tag != LBER_DEFAULT || len != 0 ) {
		tag = LBER_ERROR;
		goto error;
	}
	tag = 0;

	st_len = 0;
	if ( !BER_BVISNULL( &sessionSourceIp ) ) {
		st_len += STRLENOF( "IP=" ) + sessionSourceIp.bv_len;
	}
	if ( !BER_BVISNULL( &sessionSourceName ) ) {
		if ( st_len ) st_len++;
		st_len += STRLENOF( "NAME=" ) + sessionSourceName.bv_len;
	}
	if ( !BER_BVISNULL( &sessionTrackingIdentifier ) ) {
		if ( st_len ) st_len++;
		st_len += formatOID.bv_len + STRLENOF( "=" )
			+ sessionTrackingIdentifier.bv_len;
	}

	if ( st_len == 0 ) {
		goto error;
	}

	st_len += STRLENOF( " []" );
	st_pos = strlen( op->o_log_prefix );

	if ( sizeof( op->o_log_prefix ) - st_pos > st_len ) {
		char	*ptr = &op->o_log_prefix[ st_pos ];

		ptr = lutil_strcopy( ptr, " [" /*]*/ );

		st_len = 0;
		if ( !BER_BVISNULL( &sessionSourceIp ) ) {
			ptr = lutil_strcopy( ptr, "IP=" );
			ptr = lutil_strcopy( ptr, sessionSourceIp.bv_val );
			st_len++;
		}

		if ( !BER_BVISNULL( &sessionSourceName ) ) {
			if ( st_len ) *ptr++ = ' ';
			ptr = lutil_strcopy( ptr, "NAME=" );
			ptr = lutil_strcopy( ptr, sessionSourceName.bv_val );
			st_len++;
		}

		if ( !BER_BVISNULL( &sessionTrackingIdentifier ) ) {
			if ( st_len ) *ptr++ = ' ';
			ptr = lutil_strcopy( ptr, formatOID.bv_val );
			*ptr++ = '=';
			ptr = lutil_strcopy( ptr, sessionTrackingIdentifier.bv_val );
		}

		*ptr++ = /*[*/ ']';
		*ptr = '\0';
	}

error:;
	(void)ber_free( ber, 1 );

	if ( tag == LBER_ERROR ) {
		rs->sr_text = "sessionTracking control decoding error";
		return LDAP_PROTOCOL_ERROR;
	}


	return rs->sr_err;
}