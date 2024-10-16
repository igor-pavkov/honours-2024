static int virtio_net_handle_mq(VirtIONet *n, uint8_t cmd,
                                struct iovec *iov, unsigned int iov_cnt)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    struct virtio_net_ctrl_mq mq;
    size_t s;
    uint16_t queues;

    s = iov_to_buf(iov, iov_cnt, 0, &mq, sizeof(mq));
    if (s != sizeof(mq)) {
        return VIRTIO_NET_ERR;
    }

    if (cmd != VIRTIO_NET_CTRL_MQ_VQ_PAIRS_SET) {
        return VIRTIO_NET_ERR;
    }

    queues = lduw_p(&mq.virtqueue_pairs);

    if (queues < VIRTIO_NET_CTRL_MQ_VQ_PAIRS_MIN ||
        queues > VIRTIO_NET_CTRL_MQ_VQ_PAIRS_MAX ||
        queues > n->max_queues ||
        !n->multiqueue) {
        return VIRTIO_NET_ERR;
    }

    n->curr_queues = queues;
    /* stop the backend before changing the number of queues to avoid handling a
     * disabled queue */
    virtio_net_set_status(vdev, vdev->status);
    virtio_net_set_queues(n);

    return VIRTIO_NET_OK;
}