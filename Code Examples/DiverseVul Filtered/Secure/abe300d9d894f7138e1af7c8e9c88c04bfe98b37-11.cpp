static void virtio_net_announce(NetClientState *nc)
{
    VirtIONet *n = qemu_get_nic_opaque(nc);
    VirtIODevice *vdev = VIRTIO_DEVICE(n);

    /*
     * Make sure the virtio migration announcement timer isn't running
     * If it is, let it trigger announcement so that we do not cause
     * confusion.
     */
    if (n->announce_timer.round) {
        return;
    }

    if (virtio_vdev_has_feature(vdev, VIRTIO_NET_F_GUEST_ANNOUNCE) &&
        virtio_vdev_has_feature(vdev, VIRTIO_NET_F_CTRL_VQ)) {
            virtio_net_announce_notify(n);
    }
}