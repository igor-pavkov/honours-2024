int virtio_queue_ready(VirtQueue *vq)
{
    return vq->vring.avail != 0;
}