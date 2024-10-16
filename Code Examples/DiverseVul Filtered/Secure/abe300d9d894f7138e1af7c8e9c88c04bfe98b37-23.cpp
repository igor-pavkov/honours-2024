static size_t virtio_net_rsc_drain_seg(VirtioNetRscChain *chain,
                                       VirtioNetRscSeg *seg)
{
    int ret;
    struct virtio_net_hdr_v1 *h;

    h = (struct virtio_net_hdr_v1 *)seg->buf;
    h->flags = 0;
    h->gso_type = VIRTIO_NET_HDR_GSO_NONE;

    if (seg->is_coalesced) {
        h->rsc.segments = seg->packets;
        h->rsc.dup_acks = seg->dup_ack;
        h->flags = VIRTIO_NET_HDR_F_RSC_INFO;
        if (chain->proto == ETH_P_IP) {
            h->gso_type = VIRTIO_NET_HDR_GSO_TCPV4;
        } else {
            h->gso_type = VIRTIO_NET_HDR_GSO_TCPV6;
        }
    }

    ret = virtio_net_do_receive(seg->nc, seg->buf, seg->size);
    QTAILQ_REMOVE(&chain->buffers, seg, next);
    g_free(seg->buf);
    g_free(seg);

    return ret;
}