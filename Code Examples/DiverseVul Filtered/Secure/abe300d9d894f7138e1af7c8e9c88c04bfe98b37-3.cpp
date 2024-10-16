static void virtio_net_vhost_status(VirtIONet *n, uint8_t status)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    NetClientState *nc = qemu_get_queue(n->nic);
    int queue_pairs = n->multiqueue ? n->max_queue_pairs : 1;
    int cvq = n->max_ncs - n->max_queue_pairs;

    if (!get_vhost_net(nc->peer)) {
        return;
    }

    if ((virtio_net_started(n, status) && !nc->peer->link_down) ==
        !!n->vhost_started) {
        return;
    }
    if (!n->vhost_started) {
        int r, i;

        if (n->needs_vnet_hdr_swap) {
            error_report("backend does not support %s vnet headers; "
                         "falling back on userspace virtio",
                         virtio_is_big_endian(vdev) ? "BE" : "LE");
            return;
        }

        /* Any packets outstanding? Purge them to avoid touching rings
         * when vhost is running.
         */
        for (i = 0;  i < queue_pairs; i++) {
            NetClientState *qnc = qemu_get_subqueue(n->nic, i);

            /* Purge both directions: TX and RX. */
            qemu_net_queue_purge(qnc->peer->incoming_queue, qnc);
            qemu_net_queue_purge(qnc->incoming_queue, qnc->peer);
        }

        if (virtio_has_feature(vdev->guest_features, VIRTIO_NET_F_MTU)) {
            r = vhost_net_set_mtu(get_vhost_net(nc->peer), n->net_conf.mtu);
            if (r < 0) {
                error_report("%uBytes MTU not supported by the backend",
                             n->net_conf.mtu);

                return;
            }
        }

        n->vhost_started = 1;
        r = vhost_net_start(vdev, n->nic->ncs, queue_pairs, cvq);
        if (r < 0) {
            error_report("unable to start vhost net: %d: "
                         "falling back on userspace virtio", -r);
            n->vhost_started = 0;
        }
    } else {
        vhost_net_stop(vdev, n->nic->ncs, queue_pairs, cvq);
        n->vhost_started = 0;
    }
}