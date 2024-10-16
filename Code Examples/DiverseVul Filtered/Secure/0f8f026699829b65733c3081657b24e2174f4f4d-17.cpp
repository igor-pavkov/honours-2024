static int is_newsgroup(const char *mbox)
{
    struct wildmat *wild;

    /* don't use personal mailboxes */
    if (!mbox || !*mbox ||
	(!strncasecmp(mbox, "INBOX", 5) && (!mbox[5] || mbox[5] == '.')) ||
	!strncmp(mbox, "user.", 5) ||
	strncmp(mbox, newsprefix, strlen(newsprefix))) return 0;

    /* check shared mailboxes against the 'newsgroups' wildmat */
    wild = newsgroups;
    while (wild->pat && wildmat(mbox, wild->pat) != 1) wild++;

    /* if we don't have a match, or its a negative match, don't use it */
    if (!wild->pat || wild->not) return 0;

    /* otherwise, its usable */
    return 1;
}