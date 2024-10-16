static int virtio_net_handle_announce(VirtIONet *n, uint8_t cmd,
                                      struct iovec *iov, unsigned int iov_cnt)
{
    trace_virtio_net_handle_announce(n->announce_timer.round);
    if (cmd == VIRTIO_NET_CTRL_ANNOUNCE_ACK &&
        n->status & VIRTIO_NET_S_ANNOUNCE) {
        n->status &= ~VIRTIO_NET_S_ANNOUNCE;
        if (n->announce_timer.round) {
            qemu_announce_timer_step(&n->announce_timer);
        }
        return VIRTIO_NET_OK;
    } else {
        return VIRTIO_NET_ERR;
    }
}