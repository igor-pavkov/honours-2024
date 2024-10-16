static int newgroup(message_data_t *msg)
{
    int r;
    char *group;
    char mailboxname[MAX_MAILBOX_BUFFER];

    /* isolate newsgroup */
    group = msg->control + 8; /* skip "newgroup" */
    while (Uisspace(*group)) group++;

    snprintf(mailboxname, sizeof(mailboxname), "%s%.*s",
	     newsprefix, (int) strcspn(group, " \t\r\n"), group);

    r = mboxlist_createmailbox(mailboxname, 0, NULL, 0,
			       newsmaster, newsmaster_authstate, 0, 0, 0);

    /* XXX check body of message for useful MIME parts */

    return r;
}