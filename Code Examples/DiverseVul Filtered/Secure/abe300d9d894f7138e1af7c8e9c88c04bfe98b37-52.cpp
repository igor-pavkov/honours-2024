static int virtio_net_vnet_post_load(void *opaque, int version_id)
{
    struct VirtIONetMigTmp *tmp = opaque;

    if (tmp->has_vnet_hdr && !peer_has_vnet_hdr(tmp->parent)) {
        error_report("virtio-net: saved image requires vnet_hdr=on");
        return -EINVAL;
    }

    return 0;
}