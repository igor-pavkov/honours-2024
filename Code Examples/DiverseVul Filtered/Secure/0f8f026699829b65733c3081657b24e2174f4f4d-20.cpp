static int newnews_cb(const char *msgid, const char *rcpt, time_t mark,
		      unsigned long uid, void *rock)
{
    static char lastid[1024];
    struct newrock *nrock = (struct newrock *) rock;

    /* We have to reset the initial state.
     * Handle it as a dirty hack.
     */
    if (!msgid) {
	lastid[0] = '\0';
	return 0;
    }

    /* Make sure we don't return duplicate msgids,
     * the message is newer than the tstamp, and
     * the message is in mailbox we serve as a newsgroup..
     */
    if (strcmp(msgid, lastid) && mark >= nrock->tstamp &&
	uid && is_newsgroup(rcpt)) {
	struct wildmat *wild = nrock->wild;

	/* see if the mailbox matches one of our specified wildmats */
	while (wild->pat && wildmat(rcpt, wild->pat) != 1) wild++;

	/* we have a match, and its not a negative match */
	if (wild->pat && !wild->not) {
	    prot_printf(nntp_out, "%s\r\n", msgid);
	    strlcpy(lastid, msgid, sizeof(lastid));
	}
    }

    return 0;
}