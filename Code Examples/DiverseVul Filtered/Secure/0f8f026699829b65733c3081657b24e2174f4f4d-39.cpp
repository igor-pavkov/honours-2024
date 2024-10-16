static void cmd_over(char *msgid, unsigned long uid, unsigned long last)
{
    uint32_t msgno, last_msgno;
    struct nntp_overview *over;
    int found = 0;

    msgno = index_finduid(group_state, uid);
    if (!msgno || index_getuid(group_state, msgno) != uid) msgno++;
    last_msgno = index_finduid(group_state, last);

    for (; msgno <= last_msgno; msgno++) {
	if (!found++)
	    prot_printf(nntp_out, "224 Overview information follows:\r\n");

	if ((over = index_overview(group_state, msgno))) {
	    char xref[8192];

	    build_xref(over->msgid, xref, sizeof(xref), 0);

	    prot_printf(nntp_out, "%lu\t%s\t%s\t%s\t%s\t%s\t%lu\t%lu\t%s\r\n",
			msgid ? 0 : over->uid,
			over->subj ? over->subj : "",
			over->from ? over->from : "",
			over->date ? over->date : "",
			over->msgid ? over->msgid : "",
			over->ref ? over->ref : "",
			over->bytes + strlen(xref) + 2, /* +2 for \r\n */
			over->lines, xref);
	}
    }

    if (found)
	prot_printf(nntp_out, ".\r\n");
    else
	prot_printf(nntp_out, "423 No such article(s) in this newsgroup\r\n");
}