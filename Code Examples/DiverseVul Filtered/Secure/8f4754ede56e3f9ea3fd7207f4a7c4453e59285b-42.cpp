void bdrv_add_close_notifier(BlockDriverState *bs, Notifier *notify)
{
    notifier_list_add(&bs->close_notifiers, notify);
}