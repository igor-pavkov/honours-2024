static int virtio_net_tx_waiting_pre_save(void *opaque)
{
    struct VirtIONetMigTmp *tmp = opaque;

    tmp->vqs_1 = tmp->parent->vqs + 1;
    tmp->curr_queue_pairs_1 = tmp->parent->curr_queue_pairs - 1;
    if (tmp->parent->curr_queue_pairs == 0) {
        tmp->curr_queue_pairs_1 = 0;
    }

    return 0;
}