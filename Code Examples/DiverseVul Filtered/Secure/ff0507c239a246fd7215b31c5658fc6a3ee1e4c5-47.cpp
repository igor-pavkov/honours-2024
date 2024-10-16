static void iscsi_reopen_commit(BDRVReopenState *reopen_state)
{
    IscsiLun *iscsilun = reopen_state->bs->opaque;

    /* the cache.direct status might have changed */
    if (iscsilun->allocmap != NULL) {
        iscsi_allocmap_init(iscsilun, reopen_state->flags);
    }
}