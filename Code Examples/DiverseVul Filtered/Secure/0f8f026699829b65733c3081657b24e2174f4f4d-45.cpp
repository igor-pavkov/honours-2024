static int read_response(struct backend *s, int force_notfatal, char **result)
{
    static char buf[2048];

    s->timeout->mark = time(NULL) + IDLE_TIMEOUT;

    if (!prot_fgets(buf, sizeof(buf), s->in)) {
	/* uh oh */
	if (s == backend_current && !force_notfatal)
	    fatal("Lost connection to selected backend", EC_UNAVAILABLE);
	proxy_downserver(s);
	return IMAP_SERVER_UNAVAILABLE;
    }

    *result = buf;
    return 0;
}