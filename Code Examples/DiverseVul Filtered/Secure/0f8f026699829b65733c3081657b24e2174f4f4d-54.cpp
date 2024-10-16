static int find_cb(const char *msgid __attribute__((unused)),
		   const char *mailbox,
		   time_t mark __attribute__((unused)),
		   unsigned long uid, void *rock)
{
    struct findrock *frock = (struct findrock *) rock;

    /* skip mailboxes that we don't serve as newsgroups */
    if (!is_newsgroup(mailbox)) return 0;

    frock->mailbox = mailbox;
    frock->uid = uid;

    return CYRUSDB_DONE;
}