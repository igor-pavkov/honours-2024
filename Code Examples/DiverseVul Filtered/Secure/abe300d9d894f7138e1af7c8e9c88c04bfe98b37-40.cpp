static bool max_queue_pairs_gt_1(void *opaque, int version_id)
{
    return VIRTIO_NET(opaque)->max_queue_pairs > 1;
}