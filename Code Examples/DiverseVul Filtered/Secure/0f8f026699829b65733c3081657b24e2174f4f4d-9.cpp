static void cmd_hdr(char *cmd, char *hdr, char *pat, char *msgid,
		    unsigned long uid, unsigned long last)
{
    int msgno, last_msgno;
    int by_msgid = (msgid != NULL);
    int found = 0;

    lcase(hdr);

    msgno = index_finduid(group_state, uid);
    if (!msgno || index_getuid(group_state, msgno) != uid) msgno++;
    last_msgno = index_finduid(group_state, last);

    for (; msgno <= last_msgno; msgno++) {
	char *body;

	if (!found++)
	    prot_printf(nntp_out, "%u Headers follow:\r\n",
			cmd[0] == 'X' ? 221 : 225);

	/* see if we're looking for metadata */
	if (hdr[0] == ':') {
	    if (!strcasecmp(":bytes", hdr)) {
		char xref[8192];
		unsigned long size = index_getsize(group_state, msgno);

		if (!by_msgid) msgid = index_get_msgid(group_state, msgno);
		build_xref(msgid, xref, sizeof(xref), 0);
		if (!by_msgid) free(msgid);

		prot_printf(nntp_out, "%lu %lu\r\n", by_msgid ? 0 : uid,
			    size + strlen(xref) + 2); /* +2 for \r\n */
	    }
	    else if (!strcasecmp(":lines", hdr))
		prot_printf(nntp_out, "%u %lu\r\n",
			    by_msgid ? 0 : index_getuid(group_state, msgno),
			    index_getlines(group_state, msgno));
	    else
		prot_printf(nntp_out, "%u \r\n",
			    by_msgid ? 0 : index_getuid(group_state, msgno));
	}
	else if (!strcmp(hdr, "xref") && !pat /* [X]HDR only */) {
	    char xref[8192];

	    if (!by_msgid) msgid = index_get_msgid(group_state, msgno);
	    build_xref(msgid, xref, sizeof(xref), 1);
	    if (!by_msgid) free(msgid);

	    prot_printf(nntp_out, "%u %s\r\n",
			by_msgid ? 0 : index_getuid(group_state, msgno), xref);
	}
	else if ((body = index_getheader(group_state, msgno, hdr)) &&
		 (!pat ||			/* [X]HDR */
		  wildmat(body, pat))) {	/* XPAT with match */
		prot_printf(nntp_out, "%u %s\r\n",
			    by_msgid ? 0 : index_getuid(group_state, msgno), body);
	}
    }

    if (found)
	prot_printf(nntp_out, ".\r\n");
    else
	prot_printf(nntp_out, "423 No such article(s) in this newsgroup\r\n");
}