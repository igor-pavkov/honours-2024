void bdrv_clear_incoming_migration_all(void)
{
    BlockDriverState *bs;

    QTAILQ_FOREACH(bs, &bdrv_states, device_list) {
        bs->open_flags = bs->open_flags & ~(BDRV_O_INCOMING);
    }
}