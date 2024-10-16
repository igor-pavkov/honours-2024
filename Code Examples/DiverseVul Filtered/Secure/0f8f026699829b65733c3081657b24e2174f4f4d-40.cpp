int service_main(int argc __attribute__((unused)),
		 char **argv __attribute__((unused)),
		 char **envp __attribute__((unused)))
{
    socklen_t salen;
    char localip[60], remoteip[60];
    char hbuf[NI_MAXHOST];
    int niflags;
    sasl_security_properties_t *secprops=NULL;
    int shutdown;
    char unavail[1024];

    signals_poll();

    sync_log_init();

    nntp_in = prot_new(0, 0);
    nntp_out = prot_new(1, 1);
    protgroup_insert(protin, nntp_in);

    /* Find out name of client host */
    salen = sizeof(nntp_remoteaddr);
    if (getpeername(0, (struct sockaddr *)&nntp_remoteaddr, &salen) == 0 &&
	(nntp_remoteaddr.ss_family == AF_INET ||
	 nntp_remoteaddr.ss_family == AF_INET6)) {
	if (getnameinfo((struct sockaddr *)&nntp_remoteaddr, salen,
			hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD) == 0) {
	    strncpy(nntp_clienthost, hbuf, sizeof(hbuf));
	    strlcat(nntp_clienthost, " ", sizeof(nntp_clienthost));
	    nntp_clienthost[sizeof(nntp_clienthost)-30] = '\0';
	} else {
	    nntp_clienthost[0] = '\0';
	}
	niflags = NI_NUMERICHOST;
#ifdef NI_WITHSCOPEID
	if (((struct sockaddr *)&nntp_remoteaddr)->sa_family == AF_INET6)
	    niflags |= NI_WITHSCOPEID;
#endif
	if (getnameinfo((struct sockaddr *)&nntp_remoteaddr, salen, hbuf,
			sizeof(hbuf), NULL, 0, niflags) != 0)
	    strlcpy(hbuf, "unknown", sizeof(hbuf));
	strlcat(nntp_clienthost, "[", sizeof(nntp_clienthost));
	strlcat(nntp_clienthost, hbuf, sizeof(nntp_clienthost));
	strlcat(nntp_clienthost, "]", sizeof(nntp_clienthost));
	salen = sizeof(nntp_localaddr);
	if (getsockname(0, (struct sockaddr *)&nntp_localaddr, &salen) == 0) {
	    nntp_haveaddr = 1;
	}

	/* Create pre-authentication telemetry log based on client IP */
	nntp_logfd = telemetry_log(hbuf, nntp_in, nntp_out, 0);
    }

    /* other params should be filled in */
    if (sasl_server_new("nntp", config_servername, NULL, NULL, NULL,
			NULL, SASL_SUCCESS_DATA, &nntp_saslconn) != SASL_OK)
	fatal("SASL failed initializing: sasl_server_new()",EC_TEMPFAIL); 

    /* will always return something valid */
    secprops = mysasl_secprops(0);
    sasl_setprop(nntp_saslconn, SASL_SEC_PROPS, secprops);
    sasl_setprop(nntp_saslconn, SASL_SSF_EXTERNAL, &extprops_ssf);
    
    if(iptostring((struct sockaddr *)&nntp_localaddr, salen,
		  localip, 60) == 0) {
	sasl_setprop(nntp_saslconn, SASL_IPLOCALPORT, localip);
	saslprops.iplocalport = xstrdup(localip);
    }
    
    if(iptostring((struct sockaddr *)&nntp_remoteaddr, salen,
		  remoteip, 60) == 0) {
	sasl_setprop(nntp_saslconn, SASL_IPREMOTEPORT, remoteip);  
	saslprops.ipremoteport = xstrdup(remoteip);
    }

    proc_register("nntpd", nntp_clienthost, NULL, NULL);

    /* Set inactivity timer */
    nntp_timeout = config_getint(IMAPOPT_NNTPTIMEOUT);
    if (nntp_timeout < 3) nntp_timeout = 3;
    nntp_timeout *= 60;
    prot_settimeout(nntp_in, nntp_timeout);
    prot_setflushonread(nntp_in, nntp_out);

    /* we were connected on nntps port so we should do 
       TLS negotiation immediatly */
    if (nntps == 1) cmd_starttls(1);

    if ((shutdown = shutdown_file(unavail, sizeof(unavail)))) {
	prot_printf(nntp_out, "%u", 400);
    } else {
	prot_printf(nntp_out, "%u", (nntp_capa & MODE_READ) ? 200 : 201);
    }
    if (config_serverinfo) prot_printf(nntp_out, " %s", config_servername);
    if (config_serverinfo == IMAP_ENUM_SERVERINFO_ON) {
	prot_printf(nntp_out, " Cyrus NNTP%s %s",
		    config_mupdate_server ? " Murder" : "", cyrus_version());
    }
    if (shutdown) {
	prot_printf(nntp_out, "server unavailable, %s\r\n", unavail);
	shut_down(0);
    }
    else {
	prot_printf(nntp_out, " server ready, posting %s\r\n",
		    (nntp_capa & MODE_READ) ? "allowed" : "prohibited");
    }

    cmdloop();

    /* QUIT executed */

    /* cleanup */
    nntp_reset();

    return 0;
}