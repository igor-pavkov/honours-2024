static bool virtio_net_guest_notifier_pending(VirtIODevice *vdev, int idx)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    NetClientState *nc = qemu_get_subqueue(n->nic, vq2q(idx));
    assert(n->vhost_started);
    return vhost_net_virtqueue_pending(tap_get_vhost_net(nc->peer), idx);
}