EXPORTED int mboxlist_createsync(const char *name, int mbtype,
                        const char *partition,
                        const char *userid, const struct auth_state *auth_state,
                        int options, unsigned uidvalidity,
                        modseq_t highestmodseq,
                        const char *acl, const char *uniqueid,
                        int local_only, struct mailbox **mboxptr)
{
    return mboxlist_createmailbox_full(name, mbtype, partition,
                                       1, userid, auth_state,
                                       options, uidvalidity,
                                       highestmodseq, acl, uniqueid,
                                       local_only, 1, 0, mboxptr);
}