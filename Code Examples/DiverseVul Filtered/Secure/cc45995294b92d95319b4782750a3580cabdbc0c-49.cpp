uint16_t virtio_queue_vector(VirtIODevice *vdev, int n)
{
    return n < VIRTIO_PCI_QUEUE_MAX ? vdev->vq[n].vector :
        VIRTIO_NO_VECTOR;
}