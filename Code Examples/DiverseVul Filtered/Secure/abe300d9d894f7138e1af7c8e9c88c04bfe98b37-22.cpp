static bool has_ctrl_guest_offloads(void *opaque, int version_id)
{
    return virtio_vdev_has_feature(VIRTIO_DEVICE(opaque),
                                   VIRTIO_NET_F_CTRL_GUEST_OFFLOADS);
}