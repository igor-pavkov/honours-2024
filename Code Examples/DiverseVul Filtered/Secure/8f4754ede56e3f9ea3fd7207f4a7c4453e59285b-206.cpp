const char *bdrv_get_device_name(BlockDriverState *bs)
{
    return bs->device_name;
}