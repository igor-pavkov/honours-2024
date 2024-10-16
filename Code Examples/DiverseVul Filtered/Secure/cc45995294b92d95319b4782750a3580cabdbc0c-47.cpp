void virtio_queue_set_last_avail_idx(VirtIODevice *vdev, int n, uint16_t idx)
{
    vdev->vq[n].last_avail_idx = idx;
}