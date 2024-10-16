static void virtio_net_announce_notify(VirtIONet *net)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(net);
    trace_virtio_net_announce_notify();

    net->status |= VIRTIO_NET_S_ANNOUNCE;
    virtio_notify_config(vdev);
}