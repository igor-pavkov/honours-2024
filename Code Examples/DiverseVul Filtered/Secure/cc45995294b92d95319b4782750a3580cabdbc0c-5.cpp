int virtio_queue_get_id(VirtQueue *vq)
{
    VirtIODevice *vdev = vq->vdev;
    assert(vq >= &vdev->vq[0] && vq < &vdev->vq[VIRTIO_PCI_QUEUE_MAX]);
    return vq - &vdev->vq[0];
}