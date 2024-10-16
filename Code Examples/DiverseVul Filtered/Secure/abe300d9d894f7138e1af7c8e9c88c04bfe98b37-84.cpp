static int32_t virtio_net_rsc_coalesce4(VirtioNetRscChain *chain,
                                        VirtioNetRscSeg *seg,
                                        const uint8_t *buf, size_t size,
                                        VirtioNetRscUnit *unit)
{
    struct ip_header *ip1, *ip2;

    ip1 = (struct ip_header *)(unit->ip);
    ip2 = (struct ip_header *)(seg->unit.ip);
    if ((ip1->ip_src ^ ip2->ip_src) || (ip1->ip_dst ^ ip2->ip_dst)
        || (unit->tcp->th_sport ^ seg->unit.tcp->th_sport)
        || (unit->tcp->th_dport ^ seg->unit.tcp->th_dport)) {
        chain->stat.no_match++;
        return RSC_NO_MATCH;
    }

    return virtio_net_rsc_coalesce_data(chain, seg, buf, unit);
}