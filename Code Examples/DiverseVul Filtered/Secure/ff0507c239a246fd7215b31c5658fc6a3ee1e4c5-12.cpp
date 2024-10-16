static void iscsi_block_init(void)
{
    bdrv_register(&bdrv_iscsi);
#if LIBISCSI_API_VERSION >= (20160603)
    bdrv_register(&bdrv_iser);
#endif
}