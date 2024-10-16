static ssize_t virtio_net_do_receive(NetClientState *nc, const uint8_t *buf,
                                  size_t size)
{
    RCU_READ_LOCK_GUARD();

    return virtio_net_receive_rcu(nc, buf, size, false);
}