static int virtio_net_tx_waiting_pre_load(void *opaque)
{
    struct VirtIONetMigTmp *tmp = opaque;

    /* Reuse the pointer setup from save */
    virtio_net_tx_waiting_pre_save(opaque);

    if (tmp->parent->curr_queue_pairs > tmp->parent->max_queue_pairs) {
        error_report("virtio-net: curr_queue_pairs %x > max_queue_pairs %x",
            tmp->parent->curr_queue_pairs, tmp->parent->max_queue_pairs);

        return -EINVAL;
    }

    return 0; /* all good */
}