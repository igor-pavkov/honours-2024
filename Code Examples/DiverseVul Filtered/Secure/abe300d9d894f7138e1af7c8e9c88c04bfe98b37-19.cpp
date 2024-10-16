static void virtio_net_set_queue_pairs(VirtIONet *n)
{
    int i;
    int r;

    if (n->nic->peer_deleted) {
        return;
    }

    for (i = 0; i < n->max_queue_pairs; i++) {
        if (i < n->curr_queue_pairs) {
            r = peer_attach(n, i);
            assert(!r);
        } else {
            r = peer_detach(n, i);
            assert(!r);
        }
    }
}