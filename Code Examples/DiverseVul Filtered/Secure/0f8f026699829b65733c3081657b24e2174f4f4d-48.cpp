static int rmgroup(message_data_t *msg)
{
    int r;
    char *group;
    char mailboxname[MAX_MAILBOX_BUFFER];

    /* isolate newsgroup */
    group = msg->control + 7; /* skip "rmgroup" */
    while (Uisspace(*group)) group++;

    snprintf(mailboxname, sizeof(mailboxname), "%s%.*s",
	     newsprefix, (int) strcspn(group, " \t\r\n"), group);

    /* skip mailboxes that we don't serve as newsgroups */
    if (!is_newsgroup(mailboxname)) r = IMAP_MAILBOX_NONEXISTENT;

    /* XXX should we delete right away, or wait until empty? */

    if (!r) r = mboxlist_deletemailbox(mailboxname, 0,
				       newsmaster, newsmaster_authstate,
				       1, 0, 0);

    if (!r) sync_log_mailbox(mailboxname);

    return r;
}