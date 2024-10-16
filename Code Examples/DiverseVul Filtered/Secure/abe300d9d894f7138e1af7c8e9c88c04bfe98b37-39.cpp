static void receive_header(VirtIONet *n, const struct iovec *iov, int iov_cnt,
                           const void *buf, size_t size)
{
    if (n->has_vnet_hdr) {
        /* FIXME this cast is evil */
        void *wbuf = (void *)buf;
        work_around_broken_dhclient(wbuf, wbuf + n->host_hdr_len,
                                    size - n->host_hdr_len);

        if (n->needs_vnet_hdr_swap) {
            virtio_net_hdr_swap(VIRTIO_DEVICE(n), wbuf);
        }
        iov_from_buf(iov, iov_cnt, 0, buf, sizeof(struct virtio_net_hdr));
    } else {
        struct virtio_net_hdr hdr = {
            .flags = 0,
            .gso_type = VIRTIO_NET_HDR_GSO_NONE
        };
        iov_from_buf(iov, iov_cnt, 0, &hdr, sizeof hdr);
    }
}