hwaddr virtio_queue_get_ring_size(VirtIODevice *vdev, int n)
{
    return vdev->vq[n].vring.used - vdev->vq[n].vring.desc +
	    virtio_queue_get_used_size(vdev, n);
}