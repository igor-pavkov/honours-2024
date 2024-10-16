static struct wildmat *split_wildmats(char *str)
{
    const char *prefix;
    char pattern[MAX_MAILBOX_BUFFER] = "", *p, *c;
    struct wildmat *wild = NULL;
    int n = 0;

    if ((prefix = config_getstring(IMAPOPT_NEWSPREFIX)))
	snprintf(pattern, sizeof(pattern), "%s.", prefix);
    p = pattern + strlen(pattern);

    /*
     * split the list of wildmats
     *
     * we split them right to left because this is the order in which
     * we want to test them (per RFC3977 section 4.2)
     */
    do {
	if ((c = strrchr(str, ',')))
	    *c++ = '\0';
	else
	    c = str;

	if (!(n % 10)) /* alloc some more */
	    wild = xrealloc(wild, (n + 11) * sizeof(struct wildmat));

	if (*c == '!') wild[n].not = 1;		/* not */
	else if (*c == '@') wild[n].not = -1;	/* absolute not (feeding) */
	else wild[n].not = 0;

	strcpy(p, wild[n].not ? c + 1 : c);
	wild[n++].pat = xstrdup(pattern);
    } while (c != str);
    wild[n].pat = NULL;

    return wild;
}