EXPORTED int mboxlist_setquotas(const char *root,
                       quota_t newquotas[QUOTA_NUMRESOURCES], int force)
{
    struct quota q;
    int r;
    int res;
    struct txn *tid = NULL;
    struct mboxevent *mboxevents = NULL;
    struct mboxevent *quotachange_event = NULL;
    struct mboxevent *quotawithin_event = NULL;

    if (!root[0] || root[0] == '.' || strchr(root, '/')
        || strchr(root, '*') || strchr(root, '%') || strchr(root, '?')) {
        return IMAP_MAILBOX_BADNAME;
    }

    quota_init(&q, root);
    r = quota_read(&q, &tid, 1);

    if (!r) {
        int changed = 0;
        int underquota;

        /* has it changed? */
        for (res = 0 ; res < QUOTA_NUMRESOURCES ; res++) {
            if (q.limits[res] != newquotas[res]) {
                underquota = 0;

                /* Prepare a QuotaChange event notification *now*.
                 *
                 * This is to ensure the QuotaChange is emitted before the
                 * subsequent QuotaWithin (if the latter becomes applicable).
                 */
                if (quotachange_event == NULL) {
                    quotachange_event = mboxevent_enqueue(EVENT_QUOTA_CHANGE,
                                                          &mboxevents);
                }

                /* prepare a QuotaWithin event notification if now under quota */
                if (quota_is_overquota(&q, res, NULL) &&
                    (!quota_is_overquota(&q, res, newquotas) || newquotas[res] == -1)) {
                    if (quotawithin_event == NULL)
                        quotawithin_event = mboxevent_enqueue(EVENT_QUOTA_WITHIN,
                                                              &mboxevents);
                    underquota++;
                }

                q.limits[res] = newquotas[res];
                changed++;

                mboxevent_extract_quota(quotachange_event, &q, res);
                if (underquota)
                    mboxevent_extract_quota(quotawithin_event, &q, res);
            }
        }
        if (changed) {
            r = quota_write(&q, &tid);

            if (quotachange_event == NULL) {
                quotachange_event = mboxevent_enqueue(EVENT_QUOTA_CHANGE, &mboxevents);
            }

            for (res = 0; res < QUOTA_NUMRESOURCES; res++) {
                mboxevent_extract_quota(quotachange_event, &q, res);
            }
        }

        if (!r)
            quota_commit(&tid);

        goto done;
    }

    if (r != IMAP_QUOTAROOT_NONEXISTENT)
        goto done;

    if (config_virtdomains && root[strlen(root)-1] == '!') {
        /* domain quota */
    }
    else {
        mbentry_t *mbentry = NULL;

        /* look for a top-level mailbox in the proposed quotaroot */
        r = mboxlist_lookup(root, &mbentry, NULL);
        if (r) {
            if (!force && r == IMAP_MAILBOX_NONEXISTENT) {
                mboxlist_mboxtree(root, exists_cb, &force, MBOXTREE_SKIP_ROOT);
            }
            /* are we going to force the create anyway? */
            if (force) {
                r = 0;
            }
        }
        else if (mbentry->mbtype & (MBTYPE_REMOTE | MBTYPE_MOVING)) {
            /* Can't set quota on a remote mailbox */
            r = IMAP_MAILBOX_NOTSUPPORTED;
        }
        mboxlist_entry_free(&mbentry);
        if (r) goto done;
    }

    /* safe against quota -f and other root change races */
    r = quota_changelock();
    if (r) goto done;

    /* initialise the quota */
    memcpy(q.limits, newquotas, sizeof(q.limits));
    r = quota_write(&q, &tid);
    if (r) goto done;

    /* prepare a QuotaChange event notification */
    if (quotachange_event == NULL)
        quotachange_event = mboxevent_enqueue(EVENT_QUOTA_CHANGE, &mboxevents);

    for (res = 0; res < QUOTA_NUMRESOURCES; res++) {
        mboxevent_extract_quota(quotachange_event, &q, res);
    }

    quota_commit(&tid);

    /* recurse through mailboxes, setting the quota and finding
     * out the usage */
    mboxlist_mboxtree(root, mboxlist_changequota, (void *)root, 0);

    quota_changelockrelease();

done:
    quota_free(&q);
    if (r && tid) quota_abort(&tid);
    if (!r) {
        sync_log_quota(root);

        /* send QuotaChange and QuotaWithin event notifications */
        mboxevent_notify(&mboxevents);
    }
    mboxevent_freequeue(&mboxevents);

    return r;
}