static void cmd_article(int part, char *msgid, unsigned long uid)
{
    int msgno, by_msgid = (msgid != NULL);
    char *fname;
    FILE *msgfile;

    msgno = index_finduid(group_state, uid);
    if (!msgno || index_getuid(group_state, msgno) != uid) {
	prot_printf(nntp_out, "423 No such article in this newsgroup\r\n");
	return;
    }

    fname = mailbox_message_fname(group_state->mailbox, uid);

    msgfile = fopen(fname, "r");
    if (!msgfile) {
	prot_printf(nntp_out, "502 Could not read message file\r\n");
	return;
    }

    if (!by_msgid) {
	nntp_current = msgno;
	msgid = index_get_msgid(group_state, msgno);
    }

    prot_printf(nntp_out, "%u %lu %s\r\n",
		220 + part, by_msgid ? 0 : uid, msgid ? msgid : "<0>");

    if (part != ARTICLE_STAT) {
	char buf[4096];
	int body = 0;
	int output = (part != ARTICLE_BODY);

	while (fgets(buf, sizeof(buf), msgfile)) {
	    if (!body && buf[0] == '\r' && buf[1] == '\n') {
		/* blank line between header and body */
		body = 1;
		if (output) {
		    /* add the Xref header */
		    char xref[8192];

		    build_xref(msgid, xref, sizeof(xref), 0);
		    prot_printf(nntp_out, "%s\r\n", xref);
		}
		if (part == ARTICLE_HEAD) {
		    /* we're done */
		    break;
		}
		else if (part == ARTICLE_BODY) {
		    /* start outputing text */
		    output = 1;
		    continue;
		}
	    }

	    if (output) {
		if (buf[0] == '.') prot_putc('.', nntp_out);
		do {
		    prot_printf(nntp_out, "%s", buf);
		} while (buf[strlen(buf)-1] != '\n' &&
			 fgets(buf, sizeof(buf), msgfile));
	    }
	}

	/* Protect against messages not ending in CRLF */
	if (buf[strlen(buf)-1] != '\n') prot_printf(nntp_out, "\r\n");

	prot_printf(nntp_out, ".\r\n");

	/* Reset inactivity timer in case we spend a long time
	   pushing data to the client over a slow link. */
	prot_resettimeout(nntp_in);
    }

    if (!by_msgid) free(msgid);

    fclose(msgfile);
}