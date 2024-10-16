void bdrv_reopen_commit(BDRVReopenState *reopen_state)
{
    BlockDriver *drv;

    assert(reopen_state != NULL);
    drv = reopen_state->bs->drv;
    assert(drv != NULL);

    /* If there are any driver level actions to take */
    if (drv->bdrv_reopen_commit) {
        drv->bdrv_reopen_commit(reopen_state);
    }

    /* set BDS specific flags now */
    reopen_state->bs->open_flags         = reopen_state->flags;
    reopen_state->bs->enable_write_cache = !!(reopen_state->flags &
                                              BDRV_O_CACHE_WB);
    reopen_state->bs->read_only = !(reopen_state->flags & BDRV_O_RDWR);

    bdrv_refresh_limits(reopen_state->bs);
}