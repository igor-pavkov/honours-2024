static int virtio_net_handle_offloads(VirtIONet *n, uint8_t cmd,
                                     struct iovec *iov, unsigned int iov_cnt)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    uint64_t offloads;
    size_t s;

    if (!virtio_vdev_has_feature(vdev, VIRTIO_NET_F_CTRL_GUEST_OFFLOADS)) {
        return VIRTIO_NET_ERR;
    }

    s = iov_to_buf(iov, iov_cnt, 0, &offloads, sizeof(offloads));
    if (s != sizeof(offloads)) {
        return VIRTIO_NET_ERR;
    }

    if (cmd == VIRTIO_NET_CTRL_GUEST_OFFLOADS_SET) {
        uint64_t supported_offloads;

        offloads = virtio_ldq_p(vdev, &offloads);

        if (!n->has_vnet_hdr) {
            return VIRTIO_NET_ERR;
        }

        n->rsc4_enabled = virtio_has_feature(offloads, VIRTIO_NET_F_RSC_EXT) &&
            virtio_has_feature(offloads, VIRTIO_NET_F_GUEST_TSO4);
        n->rsc6_enabled = virtio_has_feature(offloads, VIRTIO_NET_F_RSC_EXT) &&
            virtio_has_feature(offloads, VIRTIO_NET_F_GUEST_TSO6);
        virtio_clear_feature(&offloads, VIRTIO_NET_F_RSC_EXT);

        supported_offloads = virtio_net_supported_guest_offloads(n);
        if (offloads & ~supported_offloads) {
            return VIRTIO_NET_ERR;
        }

        n->curr_guest_offloads = offloads;
        virtio_net_apply_guest_offloads(n);

        return VIRTIO_NET_OK;
    } else {
        return VIRTIO_NET_ERR;
    }
}