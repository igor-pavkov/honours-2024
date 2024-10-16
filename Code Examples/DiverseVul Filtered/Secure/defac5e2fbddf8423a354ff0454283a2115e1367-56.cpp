static BlockBackend *blk_create_empty_drive(void)
{
    return blk_new(qemu_get_aio_context(), 0, BLK_PERM_ALL);
}