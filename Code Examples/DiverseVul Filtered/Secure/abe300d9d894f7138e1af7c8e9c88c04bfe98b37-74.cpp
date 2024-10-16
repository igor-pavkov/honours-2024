static int virtio_net_vnet_pre_save(void *opaque)
{
    struct VirtIONetMigTmp *tmp = opaque;

    tmp->has_vnet_hdr = tmp->parent->has_vnet_hdr;

    return 0;
}