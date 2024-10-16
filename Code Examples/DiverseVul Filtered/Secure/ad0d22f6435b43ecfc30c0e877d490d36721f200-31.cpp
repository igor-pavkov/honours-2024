void flow_wildcards_init_for_packet(struct flow_wildcards *wc,
                                    const struct flow *flow)
{
    memset(&wc->masks, 0x0, sizeof wc->masks);

    /* Update this function whenever struct flow changes. */
    BUILD_ASSERT_DECL(FLOW_WC_SEQ == 36);

    if (flow_tnl_dst_is_set(&flow->tunnel)) {
        if (flow->tunnel.flags & FLOW_TNL_F_KEY) {
            WC_MASK_FIELD(wc, tunnel.tun_id);
        }
        WC_MASK_FIELD(wc, tunnel.ip_src);
        WC_MASK_FIELD(wc, tunnel.ip_dst);
        WC_MASK_FIELD(wc, tunnel.ipv6_src);
        WC_MASK_FIELD(wc, tunnel.ipv6_dst);
        WC_MASK_FIELD(wc, tunnel.flags);
        WC_MASK_FIELD(wc, tunnel.ip_tos);
        WC_MASK_FIELD(wc, tunnel.ip_ttl);
        WC_MASK_FIELD(wc, tunnel.tp_src);
        WC_MASK_FIELD(wc, tunnel.tp_dst);
        WC_MASK_FIELD(wc, tunnel.gbp_id);
        WC_MASK_FIELD(wc, tunnel.gbp_flags);

        if (!(flow->tunnel.flags & FLOW_TNL_F_UDPIF)) {
            if (flow->tunnel.metadata.present.map) {
                wc->masks.tunnel.metadata.present.map =
                                              flow->tunnel.metadata.present.map;
                WC_MASK_FIELD(wc, tunnel.metadata.opts.u8);
                WC_MASK_FIELD(wc, tunnel.metadata.tab);
            }
        } else {
            WC_MASK_FIELD(wc, tunnel.metadata.present.len);
            memset(wc->masks.tunnel.metadata.opts.gnv, 0xff,
                   flow->tunnel.metadata.present.len);
        }
    } else if (flow->tunnel.tun_id) {
        WC_MASK_FIELD(wc, tunnel.tun_id);
    }

    /* metadata, regs, and conj_id wildcarded. */

    WC_MASK_FIELD(wc, skb_priority);
    WC_MASK_FIELD(wc, pkt_mark);
    WC_MASK_FIELD(wc, ct_state);
    WC_MASK_FIELD(wc, ct_zone);
    WC_MASK_FIELD(wc, ct_mark);
    WC_MASK_FIELD(wc, ct_label);
    WC_MASK_FIELD(wc, recirc_id);
    WC_MASK_FIELD(wc, dp_hash);
    WC_MASK_FIELD(wc, in_port);

    /* actset_output wildcarded. */

    WC_MASK_FIELD(wc, dl_dst);
    WC_MASK_FIELD(wc, dl_src);
    WC_MASK_FIELD(wc, dl_type);
    WC_MASK_FIELD(wc, vlan_tci);

    if (flow->dl_type == htons(ETH_TYPE_IP)) {
        WC_MASK_FIELD(wc, nw_src);
        WC_MASK_FIELD(wc, nw_dst);
    } else if (flow->dl_type == htons(ETH_TYPE_IPV6)) {
        WC_MASK_FIELD(wc, ipv6_src);
        WC_MASK_FIELD(wc, ipv6_dst);
        WC_MASK_FIELD(wc, ipv6_label);
    } else if (flow->dl_type == htons(ETH_TYPE_ARP) ||
               flow->dl_type == htons(ETH_TYPE_RARP)) {
        WC_MASK_FIELD(wc, nw_src);
        WC_MASK_FIELD(wc, nw_dst);
        WC_MASK_FIELD(wc, nw_proto);
        WC_MASK_FIELD(wc, arp_sha);
        WC_MASK_FIELD(wc, arp_tha);
        return;
    } else if (eth_type_mpls(flow->dl_type)) {
        for (int i = 0; i < FLOW_MAX_MPLS_LABELS; i++) {
            WC_MASK_FIELD(wc, mpls_lse[i]);
            if (flow->mpls_lse[i] & htonl(MPLS_BOS_MASK)) {
                break;
            }
        }
        return;
    } else {
        return; /* Unknown ethertype. */
    }

    /* IPv4 or IPv6. */
    WC_MASK_FIELD(wc, nw_frag);
    WC_MASK_FIELD(wc, nw_tos);
    WC_MASK_FIELD(wc, nw_ttl);
    WC_MASK_FIELD(wc, nw_proto);

    /* No transport layer header in later fragments. */
    if (!(flow->nw_frag & FLOW_NW_FRAG_LATER) &&
        (flow->nw_proto == IPPROTO_ICMP ||
         flow->nw_proto == IPPROTO_ICMPV6 ||
         flow->nw_proto == IPPROTO_TCP ||
         flow->nw_proto == IPPROTO_UDP ||
         flow->nw_proto == IPPROTO_SCTP ||
         flow->nw_proto == IPPROTO_IGMP)) {
        WC_MASK_FIELD(wc, tp_src);
        WC_MASK_FIELD(wc, tp_dst);

        if (flow->nw_proto == IPPROTO_TCP) {
            WC_MASK_FIELD(wc, tcp_flags);
        } else if (flow->nw_proto == IPPROTO_ICMPV6) {
            WC_MASK_FIELD(wc, arp_sha);
            WC_MASK_FIELD(wc, arp_tha);
            WC_MASK_FIELD(wc, nd_target);
        } else if (flow->nw_proto == IPPROTO_IGMP) {
            WC_MASK_FIELD(wc, igmp_group_ip4);
        }
    }
}