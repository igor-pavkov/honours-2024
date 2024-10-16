static ssize_t virtio_net_receive(NetClientState *nc, const uint8_t *buf,
                                  size_t size)
{
    VirtIONet *n = qemu_get_nic_opaque(nc);
    if ((n->rsc4_enabled || n->rsc6_enabled)) {
        return virtio_net_rsc_receive(nc, buf, size);
    } else {
        return virtio_net_do_receive(nc, buf, size);
    }
}