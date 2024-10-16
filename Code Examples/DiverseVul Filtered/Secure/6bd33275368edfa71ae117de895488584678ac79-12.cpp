static int mboxlist_update_entry(const char *name, const mbentry_t *mbentry, struct txn **txn)
{
    mbentry_t *old = NULL;
    int r = 0;

    mboxlist_mylookup(name, &old, txn, 0); // ignore errors, it will be NULL

    if (!cyrusdb_fetch(mbdb, "$RACL", 5, NULL, NULL, txn)) {
        r = mboxlist_update_racl(name, old, mbentry, txn);
        /* XXX return value here is discarded? */
    }

    if (mbentry) {
        char *mboxent = mboxlist_entry_cstring(mbentry);
        r = cyrusdb_store(mbdb, name, strlen(name), mboxent, strlen(mboxent), txn);
        free(mboxent);

        if (!r && config_auditlog) {
            /* XXX is there a difference between "" and NULL? */
            if (old && strcmpsafe(old->acl, mbentry->acl)) {
                syslog(LOG_NOTICE, "auditlog: acl sessionid=<%s> "
                                   "mailbox=<%s> uniqueid=<%s> "
                                   "oldacl=<%s> acl=<%s>",
                       session_id(),
                       name, mbentry->uniqueid,
                       old->acl, mbentry->acl);
            }
        }
    }
    else {
        r = cyrusdb_delete(mbdb, name, strlen(name), txn, /*force*/1);
    }

    mboxlist_entry_free(&old);
    return r;
}