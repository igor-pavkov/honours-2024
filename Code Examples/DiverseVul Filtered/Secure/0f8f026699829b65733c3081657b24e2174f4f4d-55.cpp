static int strip_post_addresses(char *body)
{
    const char *newspostuser = config_getstring(IMAPOPT_NEWSPOSTUSER);
    char *p, *end;
    size_t postlen, n;
    int nonpost = 0;

    if (!newspostuser) return 1;  /* we didn't add this header, so leave it */
    postlen = strlen(newspostuser);

    for (p = body;; p += n) {
	end = p;

	/* skip whitespace */
	while (p && *p && (Uisspace(*p) || *p == ',')) p++;

	if (!p || !*p) break;

	/* find end of address */
	n = strcspn(p, ", \t\r\n");

	if ((n > postlen + 1) &&  /* +1 for '+' */
	    !strncmp(p, newspostuser, postlen) && p[postlen] == '+') {
	    /* found a post address.  since we always add the post
	     * addresses to the end of the header, truncate it right here.
	     */
	    strcpy(end, "\r\n");
	    break;
	}
	
	nonpost = 1;
    }

    return nonpost;
}