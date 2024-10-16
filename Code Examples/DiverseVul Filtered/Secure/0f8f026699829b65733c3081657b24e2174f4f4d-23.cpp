static void cmd_starttls(int nntps)
{
    int result;
    int *layerp;
    sasl_ssf_t ssf;
    char *auth_id;

    if (nntp_starttls_done == 1) {
	prot_printf(nntp_out, "502 %s\r\n", 
		    "TLS is already active");
	return;
    }
    if (nntp_authstate) {
	prot_printf(nntp_out, "502 %s\r\n", 
		    "Already authenticated");
	return;
    }

    /* SASL and openssl have different ideas about whether ssf is signed */
    layerp = (int *) &ssf;

    result=tls_init_serverengine("nntp",
				 5,        /* depth to verify */
				 !nntps,   /* can client auth? */
				 !nntps);  /* TLS only? */

    if (result == -1) {

	syslog(LOG_ERR, "[nntpd] error initializing TLS");

	if (nntps == 0)
	    prot_printf(nntp_out, "580 %s\r\n", "Error initializing TLS");
	else
	    fatal("tls_init() failed",EC_TEMPFAIL);

	return;
    }

    if (nntps == 0)
    {
	prot_printf(nntp_out, "382 %s\r\n", "Begin TLS negotiation now");
	/* must flush our buffers before starting tls */
	prot_flush(nntp_out);
    }
  
    result=tls_start_servertls(0, /* read */
			       1, /* write */
			       nntps ? 180 : nntp_timeout,
			       layerp,
			       &auth_id,
			       &tls_conn);

    /* if error */
    if (result==-1) {
	if (nntps == 0) {
	    prot_printf(nntp_out, "580 Starttls failed\r\n");
	    syslog(LOG_NOTICE, "[nntpd] STARTTLS failed: %s", nntp_clienthost);
	} else {
	    syslog(LOG_NOTICE, "nntps failed: %s", nntp_clienthost);
	    fatal("tls_start_servertls() failed", EC_TEMPFAIL);
	}
	return;
    }

    /* tell SASL about the negotiated layer */
    result = sasl_setprop(nntp_saslconn, SASL_SSF_EXTERNAL, &ssf);
    if (result != SASL_OK) {
	fatal("sasl_setprop() failed: cmd_starttls()", EC_TEMPFAIL);
    }
    saslprops.ssf = ssf;

    result = sasl_setprop(nntp_saslconn, SASL_AUTH_EXTERNAL, auth_id);
    if (result != SASL_OK) {
        fatal("sasl_setprop() failed: cmd_starttls()", EC_TEMPFAIL);
    }
    if(saslprops.authid) {
	free(saslprops.authid);
	saslprops.authid = NULL;
    }
    if(auth_id)
	saslprops.authid = xstrdup(auth_id);

    /* tell the prot layer about our new layers */
    prot_settls(nntp_in, tls_conn);
    prot_settls(nntp_out, tls_conn);

    nntp_starttls_done = 1;

    /* close any selected group */
    if (group_state)
	index_close(&group_state);
    if (backend_current) {
	proxy_downserver(backend_current);
	backend_current = NULL;
    }
}