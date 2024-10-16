int slap_parse_ctrl(
	Operation *op,
	SlapReply *rs,
	LDAPControl *control,
	const char **text )
{
	struct slap_control *sc;
	int rc = LDAP_SUCCESS;

	sc = find_ctrl( control->ldctl_oid );
	if( sc != NULL ) {
		/* recognized control */
		slap_mask_t tagmask;
		switch( op->o_tag ) {
		case LDAP_REQ_ADD:
			tagmask = SLAP_CTRL_ADD;
			break;
		case LDAP_REQ_BIND:
			tagmask = SLAP_CTRL_BIND;
			break;
		case LDAP_REQ_COMPARE:
			tagmask = SLAP_CTRL_COMPARE;
			break;
		case LDAP_REQ_DELETE:
			tagmask = SLAP_CTRL_DELETE;
			break;
		case LDAP_REQ_MODIFY:
			tagmask = SLAP_CTRL_MODIFY;
			break;
		case LDAP_REQ_RENAME:
			tagmask = SLAP_CTRL_RENAME;
			break;
		case LDAP_REQ_SEARCH:
			tagmask = SLAP_CTRL_SEARCH;
			break;
		case LDAP_REQ_UNBIND:
			tagmask = SLAP_CTRL_UNBIND;
			break;
		case LDAP_REQ_ABANDON:
			tagmask = SLAP_CTRL_ABANDON;
			break;
		case LDAP_REQ_EXTENDED:
			tagmask=~0L;
			assert( op->ore_reqoid.bv_val != NULL );
			if( sc->sc_extendedopsbv != NULL ) {
				int i;
				for( i=0; !BER_BVISNULL( &sc->sc_extendedopsbv[i] ); i++ ) {
					if( bvmatch( &op->ore_reqoid,
						&sc->sc_extendedopsbv[i] ) )
					{
						tagmask=0L;
						break;
					}
				}
			}
			break;
		default:
			*text = "controls internal error";
			return LDAP_OTHER;
		}

		if (( sc->sc_mask & tagmask ) == tagmask ) {
			/* available extension */
			if ( sc->sc_parse ) {
				rc = sc->sc_parse( op, rs, control );
				assert( rc != LDAP_UNAVAILABLE_CRITICAL_EXTENSION );

			} else if ( control->ldctl_iscritical ) {
				*text = "not yet implemented";
				rc = LDAP_OTHER;
			}


		} else if ( control->ldctl_iscritical ) {
			/* unavailable CRITICAL control */
			*text = "critical extension is unavailable";
			rc = LDAP_UNAVAILABLE_CRITICAL_EXTENSION;
		}

	} else if ( control->ldctl_iscritical ) {
		/* unrecognized CRITICAL control */
		*text = "critical extension is not recognized";
		rc = LDAP_UNAVAILABLE_CRITICAL_EXTENSION;
	}

	return rc;
}