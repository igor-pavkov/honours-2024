static int32_t virtio_net_rsc_coalesce_data(VirtioNetRscChain *chain,
                                            VirtioNetRscSeg *seg,
                                            const uint8_t *buf,
                                            VirtioNetRscUnit *n_unit)
{
    void *data;
    uint16_t o_ip_len;
    uint32_t nseq, oseq;
    VirtioNetRscUnit *o_unit;

    o_unit = &seg->unit;
    o_ip_len = htons(*o_unit->ip_plen);
    nseq = htonl(n_unit->tcp->th_seq);
    oseq = htonl(o_unit->tcp->th_seq);

    /* out of order or retransmitted. */
    if ((nseq - oseq) > VIRTIO_NET_MAX_TCP_PAYLOAD) {
        chain->stat.data_out_of_win++;
        return RSC_FINAL;
    }

    data = ((uint8_t *)n_unit->tcp) + n_unit->tcp_hdrlen;
    if (nseq == oseq) {
        if ((o_unit->payload == 0) && n_unit->payload) {
            /* From no payload to payload, normal case, not a dup ack or etc */
            chain->stat.data_after_pure_ack++;
            goto coalesce;
        } else {
            return virtio_net_rsc_handle_ack(chain, seg, buf,
                                             n_unit->tcp, o_unit->tcp);
        }
    } else if ((nseq - oseq) != o_unit->payload) {
        /* Not a consistent packet, out of order */
        chain->stat.data_out_of_order++;
        return RSC_FINAL;
    } else {
coalesce:
        if ((o_ip_len + n_unit->payload) > chain->max_payload) {
            chain->stat.over_size++;
            return RSC_FINAL;
        }

        /* Here comes the right data, the payload length in v4/v6 is different,
           so use the field value to update and record the new data len */
        o_unit->payload += n_unit->payload; /* update new data len */

        /* update field in ip header */
        *o_unit->ip_plen = htons(o_ip_len + n_unit->payload);

        /* Bring 'PUSH' big, the whql test guide says 'PUSH' can be coalesced
           for windows guest, while this may change the behavior for linux
           guest (only if it uses RSC feature). */
        o_unit->tcp->th_offset_flags = n_unit->tcp->th_offset_flags;

        o_unit->tcp->th_ack = n_unit->tcp->th_ack;
        o_unit->tcp->th_win = n_unit->tcp->th_win;

        memmove(seg->buf + seg->size, data, n_unit->payload);
        seg->size += n_unit->payload;
        seg->packets++;
        chain->stat.coalesced++;
        return RSC_COALESCE;
    }
}