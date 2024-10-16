static int cancel_cb(const char *msgid __attribute__((unused)),
		     const char *name,
		     time_t mark __attribute__((unused)),
		     unsigned long uid,
		     void *rock)
{
    struct mailbox *mailbox = NULL;

    /* make sure its a message in a mailbox that we're serving via NNTP */
    if (is_newsgroup(name)) {
	int r;

	r = mailbox_open_iwl(name, &mailbox);

	if (!r &&
	    !(cyrus_acl_myrights(newsmaster_authstate, mailbox->acl) & ACL_DELETEMSG))
	    r = IMAP_PERMISSION_DENIED;

	if (!r) r = mailbox_expunge(mailbox, expunge_cancelled, &uid, NULL);
	mailbox_close(&mailbox);

	/* if we failed, pass the return code back in the rock */
	if (r) *((int *) rock) = r;
    }

    return 0;
}