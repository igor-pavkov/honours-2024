void virtio_queue_set_vector(VirtIODevice *vdev, int n, uint16_t vector)
{
    if (n < VIRTIO_PCI_QUEUE_MAX)
        vdev->vq[n].vector = vector;
}