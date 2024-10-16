int gnutls_session_ticket_send(gnutls_session_t session, unsigned nr, unsigned flags)
{
	int ret = 0;
	const version_entry_st *vers = get_version(session);

	if (!vers->tls13_sem || session->security_parameters.entity == GNUTLS_CLIENT)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	if (nr == 0)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	switch (TICKET_STATE) {
	case TICKET_STATE0:
		ret = _gnutls_io_write_flush(session);
		TICKET_STATE = TICKET_STATE0;
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
		FALLTHROUGH;
	case TICKET_STATE1:
		ret =
		    _gnutls13_send_session_ticket(session, nr, TICKET_STATE==TICKET_STATE1?1:0);
		TICKET_STATE = TICKET_STATE1;
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
		break;
	default:
		gnutls_assert();
		return GNUTLS_E_INTERNAL_ERROR;
	}

	TICKET_STATE = TICKET_STATE0;

	return 0;
}