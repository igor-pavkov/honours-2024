static int pipe_to_end_of_response(struct backend *s, int force_notfatal)
{
    char buf[2048];

    s->timeout->mark = time(NULL) + IDLE_TIMEOUT;

    do {
	if (!prot_fgets(buf, sizeof(buf), s->in)) {
	    /* uh oh */
	    if (s == backend_current && !force_notfatal)
		fatal("Lost connection to selected backend", EC_UNAVAILABLE);
	    proxy_downserver(s);
	    return IMAP_SERVER_UNAVAILABLE;
	}

	prot_printf(nntp_out, "%s", buf);
    } while (strcmp(buf, ".\r\n"));

    return 0;
}