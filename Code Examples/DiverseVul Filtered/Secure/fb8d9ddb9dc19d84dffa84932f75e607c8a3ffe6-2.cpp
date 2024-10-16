int ssl3_check_finished(SSL *s)
	{
	int ok;
	long n;

	/* Read the message to see if it is supplemental data,
	 * regardless if there is a session ticket this function is
	 * called when we really expect a Certificate message, so
	 * permit appropriate message length */
	n=s->method->ssl_get_message(s,
		SSL3_ST_CR_CERT_A,
		SSL3_ST_CR_CERT_B,
		-1,
		s->max_cert_list,
		&ok);
	if (!ok) return((int)n);
	s->s3->tmp.reuse_message = 1;

	if (s->s3->tmp.message_type == SSL3_MT_SUPPLEMENTAL_DATA)
		return 3;
	/* If we have no ticket it cannot be a resumed session. */
	if (!s->session->tlsext_tick)
		return 1;
	if ((s->s3->tmp.message_type == SSL3_MT_FINISHED)
		|| (s->s3->tmp.message_type == SSL3_MT_NEWSESSION_TICKET))
		return 2;

	return 1;
	}