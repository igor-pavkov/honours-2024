static void virtio_net_set_link_status(NetClientState *nc)
{
    VirtIONet *n = qemu_get_nic_opaque(nc);
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    uint16_t old_status = n->status;

    if (nc->link_down)
        n->status &= ~VIRTIO_NET_S_LINK_UP;
    else
        n->status |= VIRTIO_NET_S_LINK_UP;

    if (n->status != old_status)
        virtio_notify_config(vdev);

    virtio_net_set_status(vdev, vdev->status);
}