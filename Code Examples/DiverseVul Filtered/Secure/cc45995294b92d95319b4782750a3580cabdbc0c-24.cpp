int virtio_queue_empty(VirtQueue *vq)
{
    return vring_avail_idx(vq) == vq->last_avail_idx;
}