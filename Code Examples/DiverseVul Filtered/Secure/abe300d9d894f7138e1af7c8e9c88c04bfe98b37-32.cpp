static void virtio_net_set_multiqueue(VirtIONet *n, int multiqueue)
{
    int max = multiqueue ? n->max_queue_pairs : 1;

    n->multiqueue = multiqueue;
    virtio_net_change_num_queue_pairs(n, max);

    virtio_net_set_queue_pairs(n);
}