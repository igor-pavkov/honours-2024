hwaddr virtio_queue_get_addr(VirtIODevice *vdev, int n)
{
    return vdev->vq[n].pa;
}