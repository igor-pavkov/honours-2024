static int32_t virtio_net_rsc_coalesce6(VirtioNetRscChain *chain,
                                        VirtioNetRscSeg *seg,
                                        const uint8_t *buf, size_t size,
                                        VirtioNetRscUnit *unit)
{
    struct ip6_header *ip1, *ip2;

    ip1 = (struct ip6_header *)(unit->ip);
    ip2 = (struct ip6_header *)(seg->unit.ip);
    if (memcmp(&ip1->ip6_src, &ip2->ip6_src, sizeof(struct in6_address))
        || memcmp(&ip1->ip6_dst, &ip2->ip6_dst, sizeof(struct in6_address))
        || (unit->tcp->th_sport ^ seg->unit.tcp->th_sport)
        || (unit->tcp->th_dport ^ seg->unit.tcp->th_dport)) {
            chain->stat.no_match++;
            return RSC_NO_MATCH;
    }

    return virtio_net_rsc_coalesce_data(chain, seg, buf, unit);
}