void virtio_queue_set_addr(VirtIODevice *vdev, int n, hwaddr addr)
{
    vdev->vq[n].pa = addr;
    virtqueue_init(&vdev->vq[n]);
}