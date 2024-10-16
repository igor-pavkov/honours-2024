static int eaptls_initiate(void *type_arg, EAP_HANDLER *handler)
{
	int		status;
	tls_session_t	*ssn;
	eap_tls_t	*inst;
	VALUE_PAIR	*vp;
	int		client_cert = TRUE;
	int		verify_mode = 0;
	REQUEST		*request = handler->request;

	inst = (eap_tls_t *)type_arg;

	handler->tls = TRUE;
	handler->finished = FALSE;

	/*
	 *	Manually flush the sessions every so often.  If HALF
	 *	of the session lifetime has passed since we last
	 *	flushed, then flush it again.
	 *
	 *	FIXME: Also do it every N sessions?
	 */
	if (inst->conf->session_cache_enable &&
	    ((inst->conf->session_last_flushed + (inst->conf->session_timeout * 1800)) <= request->timestamp)) {
		RDEBUG2("Flushing SSL sessions (of #%ld)",
			SSL_CTX_sess_number(inst->ctx));

		SSL_CTX_flush_sessions(inst->ctx, request->timestamp);
		inst->conf->session_last_flushed = request->timestamp;
	}

	/*
	 *	If we're TTLS or PEAP, then do NOT require a client
	 *	certificate.
	 *
	 *	FIXME: This should be more configurable.
	 */
	if (handler->eap_type != PW_EAP_TLS) {
		vp = pairfind(handler->request->config_items,
			      PW_EAP_TLS_REQUIRE_CLIENT_CERT);
		if (!vp) {
			client_cert = FALSE;
		} else {
			client_cert = vp->vp_integer;
		}
	}

	/*
	 *	Every new session is started only from EAP-TLS-START.
	 *	Before Sending EAP-TLS-START, open a new SSL session.
	 *	Create all the required data structures & store them
	 *	in Opaque.  So that we can use these data structures
	 *	when we get the response
	 */
	ssn = eaptls_new_session(inst->ctx, client_cert);
	if (!ssn) {
		return 0;
	}

	/*
	 *	Verify the peer certificate, if asked.
	 */
	if (client_cert) {
		RDEBUG2("Requiring client certificate");
		verify_mode = SSL_VERIFY_PEER;
		verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
		verify_mode |= SSL_VERIFY_CLIENT_ONCE;
	}
	SSL_set_verify(ssn->ssl, verify_mode, cbtls_verify);

	/*
	 *	Create a structure for all the items required to be
	 *	verified for each client and set that as opaque data
	 *	structure.
	 *
	 *	NOTE: If we want to set each item sepearately then
	 *	this index should be global.
	 */
	SSL_set_ex_data(ssn->ssl, 0, (void *)handler);
	SSL_set_ex_data(ssn->ssl, 1, (void *)inst->conf);
#ifdef HAVE_OPENSSL_OCSP_H
	SSL_set_ex_data(ssn->ssl, 2, (void *)inst->store);
#endif

	ssn->length_flag = inst->conf->include_length;

	/*
	 *	We use default fragment size, unless the Framed-MTU
	 *	tells us it's too big.  Note that we do NOT account
	 *	for the EAP-TLS headers if conf->fragment_size is
	 *	large, because that config item looks to be confusing.
	 *
	 *	i.e. it should REALLY be called MTU, and the code here
	 *	should figure out what that means for TLS fragment size.
	 *	asking the administrator to know the internal details
	 *	of EAP-TLS in order to calculate fragment sizes is
	 *	just too much.
	 */
	ssn->offset = inst->conf->fragment_size;
	vp = pairfind(handler->request->packet->vps, PW_FRAMED_MTU);
	if (vp && ((vp->vp_integer - 14) < ssn->offset)) {
		/*
		 *	Discount the Framed-MTU by:
		 *	 4 : EAPOL header
		 *	 4 : EAP header (code + id + length)
		 *	 1 : EAP type == EAP-TLS
		 *	 1 : EAP-TLS Flags
		 *	 4 : EAP-TLS Message length
		 *	    (even if conf->include_length == 0,
		 *	     just to be lazy).
		 *	---
		 *	14
		 */
		ssn->offset = vp->vp_integer - 14;
	}

	handler->opaque = ((void *)ssn);
	handler->free_opaque = session_free;

	RDEBUG2("Initiate");

	/*
	 *	Set up type-specific information.
	 */
	switch (handler->eap_type) {
	case PW_EAP_TLS:
	default:
		ssn->prf_label = "client EAP encryption";
		break;

	case PW_EAP_TTLS:
		ssn->prf_label = "ttls keying material";
		break;

		/*
		 *	PEAP-specific breakage.
		 */
	case PW_EAP_PEAP:
		/*
		 *	As it is a poorly designed protocol, PEAP uses
		 *	bits in the TLS header to indicate PEAP
		 *	version numbers.  For now, we only support
		 *	PEAP version 0, so it doesn't matter too much.
		 *	However, if we support later versions of PEAP,
		 *	we will need this flag to indicate which
		 *	version we're currently dealing with.
		 */
		ssn->peap_flag = 0x00;

		/*
		 *	PEAP version 0 requires 'include_length = no',
		 *	so rather than hoping the user figures it out,
		 *	we force it here.
		 */
		ssn->length_flag = 0;

		ssn->prf_label = "client EAP encryption";
		break;
	}

	if (inst->conf->session_cache_enable) {
		ssn->allow_session_resumption = 1; /* otherwise it's zero */
	}

	/*
	 *	TLS session initialization is over.  Now handle TLS
	 *	related handshaking or application data.
	 */
	status = eaptls_start(handler->eap_ds, ssn->peap_flag);
	RDEBUG2("Start returned %d", status);
	if (status == 0)
		return 0;

	/*
	 *	The next stage to process the packet.
	 */
	handler->stage = AUTHENTICATE;

	return 1;
}