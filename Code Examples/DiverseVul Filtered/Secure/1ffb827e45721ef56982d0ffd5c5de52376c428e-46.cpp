int gnutls_record_get_direction(gnutls_session_t session)
{
	return session->internals.direction;
}