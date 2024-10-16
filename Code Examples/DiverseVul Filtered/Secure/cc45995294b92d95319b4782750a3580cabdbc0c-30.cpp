void virtio_del_queue(VirtIODevice *vdev, int n)
{
    if (n < 0 || n >= VIRTIO_PCI_QUEUE_MAX) {
        abort();
    }

    vdev->vq[n].vring.num = 0;
}