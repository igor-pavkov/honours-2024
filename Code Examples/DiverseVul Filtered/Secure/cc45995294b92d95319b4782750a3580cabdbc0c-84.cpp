uint16_t virtio_queue_get_last_avail_idx(VirtIODevice *vdev, int n)
{
    return vdev->vq[n].last_avail_idx;
}