static int virtio_net_ufo_post_load(void *opaque, int version_id)
{
    struct VirtIONetMigTmp *tmp = opaque;

    if (tmp->has_ufo && !peer_has_ufo(tmp->parent)) {
        error_report("virtio-net: saved image requires TUN_F_UFO support");
        return -EINVAL;
    }

    return 0;
}