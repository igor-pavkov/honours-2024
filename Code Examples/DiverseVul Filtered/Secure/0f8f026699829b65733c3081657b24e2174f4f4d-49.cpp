static int mvgroup(message_data_t *msg)
{
    int r;
    size_t len;
    char *group;
    char oldmailboxname[MAX_MAILBOX_BUFFER];
    char newmailboxname[MAX_MAILBOX_BUFFER];

    /* isolate old newsgroup */
    group = msg->control + 7; /* skip "mvgroup" */
    while (Uisspace(*group)) group++;

    len = strcspn(group, " \t\r\n");
    snprintf(oldmailboxname, sizeof(oldmailboxname), "%s%.*s",
	     newsprefix, (int)len, group);

    /* isolate new newsgroup */
    group += len; /* skip old newsgroup */
    while (Uisspace(*group)) group++;

    len = strcspn(group, " \t\r\n");
    snprintf(newmailboxname, sizeof(newmailboxname), "%s%.*s",
	     newsprefix, (int)len, group);

    r = mboxlist_renamemailbox(oldmailboxname, newmailboxname, NULL, 0,
			       newsmaster, newsmaster_authstate, 0, 0);

    /* XXX check body of message for useful MIME parts */

    if (!r) sync_log_mailbox_double(oldmailboxname, newmailboxname);

    return r;
}