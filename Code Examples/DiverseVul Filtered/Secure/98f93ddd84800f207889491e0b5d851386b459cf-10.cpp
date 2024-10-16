static void virtio_net_save(QEMUFile *f, void *opaque)
{
    int i;
    VirtIONet *n = opaque;
    VirtIODevice *vdev = VIRTIO_DEVICE(n);

    /* At this point, backend must be stopped, otherwise
     * it might keep writing to memory. */
    assert(!n->vhost_started);
    virtio_save(vdev, f);

    qemu_put_buffer(f, n->mac, ETH_ALEN);
    qemu_put_be32(f, n->vqs[0].tx_waiting);
    qemu_put_be32(f, n->mergeable_rx_bufs);
    qemu_put_be16(f, n->status);
    qemu_put_byte(f, n->promisc);
    qemu_put_byte(f, n->allmulti);
    qemu_put_be32(f, n->mac_table.in_use);
    qemu_put_buffer(f, n->mac_table.macs, n->mac_table.in_use * ETH_ALEN);
    qemu_put_buffer(f, (uint8_t *)n->vlans, MAX_VLAN >> 3);
    qemu_put_be32(f, n->has_vnet_hdr);
    qemu_put_byte(f, n->mac_table.multi_overflow);
    qemu_put_byte(f, n->mac_table.uni_overflow);
    qemu_put_byte(f, n->alluni);
    qemu_put_byte(f, n->nomulti);
    qemu_put_byte(f, n->nouni);
    qemu_put_byte(f, n->nobcast);
    qemu_put_byte(f, n->has_ufo);
    if (n->max_queues > 1) {
        qemu_put_be16(f, n->max_queues);
        qemu_put_be16(f, n->curr_queues);
        for (i = 1; i < n->curr_queues; i++) {
            qemu_put_be32(f, n->vqs[i].tx_waiting);
        }
    }

    if ((1 << VIRTIO_NET_F_CTRL_GUEST_OFFLOADS) & vdev->guest_features) {
        qemu_put_be64(f, n->curr_guest_offloads);
    }
}