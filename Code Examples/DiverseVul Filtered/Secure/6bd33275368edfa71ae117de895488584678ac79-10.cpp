EXPORTED int mboxlist_createmailbox(const char *name, int mbtype,
                           const char *partition,
                           int isadmin, const char *userid,
                           const struct auth_state *auth_state,
                           int localonly, int forceuser, int dbonly,
                           int notify, struct mailbox **mailboxptr)
{
    int options = config_getint(IMAPOPT_MAILBOX_DEFAULT_OPTIONS)
                  | OPT_POP3_NEW_UIDL;
    int r;
    struct mailbox *mailbox = NULL;
    uint32_t uidvalidity = 0;

    /* check if a previous deleted mailbox existed */
    mbentry_t *oldmbentry = NULL;
    r = mboxlist_lookup_allow_all(name, &oldmbentry, NULL);
    if (!r && oldmbentry->mbtype == MBTYPE_DELETED) {
        /* then the UIDVALIDITY must be higher than before */
        if (uidvalidity <= oldmbentry->uidvalidity)
            uidvalidity = oldmbentry->uidvalidity+1;
    }
    mboxlist_entry_free(&oldmbentry);

    r = mboxlist_createmailbox_full(name, mbtype, partition,
                                    isadmin, userid, auth_state,
                                    options, uidvalidity, 0, NULL, NULL, localonly,
                                    forceuser, dbonly, &mailbox);

    if (notify && !r) {
        /* send a MailboxCreate event notification */
        struct mboxevent *mboxevent = mboxevent_new(EVENT_MAILBOX_CREATE);
        mboxevent_extract_mailbox(mboxevent, mailbox);
        mboxevent_set_access(mboxevent, NULL, NULL, userid, mailbox->name, 1);

        mboxevent_notify(&mboxevent);
        mboxevent_free(&mboxevent);
    }

    if (mailboxptr && !r) *mailboxptr = mailbox;
    else mailbox_close(&mailbox);

    return r;
}