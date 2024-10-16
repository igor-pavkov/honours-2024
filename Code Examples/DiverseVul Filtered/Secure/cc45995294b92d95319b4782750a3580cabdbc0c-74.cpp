hwaddr virtio_queue_get_ring_addr(VirtIODevice *vdev, int n)
{
    return vdev->vq[n].vring.desc;
}