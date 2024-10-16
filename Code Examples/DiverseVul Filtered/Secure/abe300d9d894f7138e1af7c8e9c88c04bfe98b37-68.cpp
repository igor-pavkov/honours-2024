static int virtio_net_rsc_tcp_ctrl_check(VirtioNetRscChain *chain,
                                         struct tcp_header *tcp)
{
    uint16_t tcp_hdr;
    uint16_t tcp_flag;

    tcp_flag = htons(tcp->th_offset_flags);
    tcp_hdr = (tcp_flag & VIRTIO_NET_TCP_HDR_LENGTH) >> 10;
    tcp_flag &= VIRTIO_NET_TCP_FLAG;
    if (tcp_flag & TH_SYN) {
        chain->stat.tcp_syn++;
        return RSC_BYPASS;
    }

    if (tcp_flag & (TH_FIN | TH_URG | TH_RST | TH_ECE | TH_CWR)) {
        chain->stat.tcp_ctrl_drain++;
        return RSC_FINAL;
    }

    if (tcp_hdr > sizeof(struct tcp_header)) {
        chain->stat.tcp_all_opt++;
        return RSC_FINAL;
    }

    return RSC_CANDIDATE;
}