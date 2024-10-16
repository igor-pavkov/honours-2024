    OVS_REQUIRES(ct->buckets[*bucket].lock)
{
    if ((ctx_in->key.dl_type == htons(ETH_TYPE_IP) &&
         !pkt->md.ct_orig_tuple.ipv4.ipv4_proto) ||
        (ctx_in->key.dl_type == htons(ETH_TYPE_IPV6) &&
         !pkt->md.ct_orig_tuple.ipv6.ipv6_proto) ||
        !(pkt->md.ct_state & (CS_SRC_NAT | CS_DST_NAT)) ||
        nat_action_info) {
        return false;
    }

    ct_lock_unlock(&ct->buckets[*bucket].lock);
    struct conn_lookup_ctx ctx;
    memset(&ctx, 0 , sizeof ctx);
    ctx.conn = NULL;

    if (ctx_in->key.dl_type == htons(ETH_TYPE_IP)) {
        ctx.key.src.addr.ipv4_aligned = pkt->md.ct_orig_tuple.ipv4.ipv4_src;
        ctx.key.dst.addr.ipv4_aligned = pkt->md.ct_orig_tuple.ipv4.ipv4_dst;

        if (ctx_in->key.nw_proto == IPPROTO_ICMP) {
            ctx.key.src.icmp_id = ctx_in->key.src.icmp_id;
            ctx.key.dst.icmp_id = ctx_in->key.dst.icmp_id;
            uint16_t src_port = ntohs(pkt->md.ct_orig_tuple.ipv4.src_port);
            ctx.key.src.icmp_type = (uint8_t) src_port;
            ctx.key.dst.icmp_type = reverse_icmp_type(ctx.key.src.icmp_type);
        } else {
            ctx.key.src.port = pkt->md.ct_orig_tuple.ipv4.src_port;
            ctx.key.dst.port = pkt->md.ct_orig_tuple.ipv4.dst_port;
        }
        ctx.key.nw_proto = pkt->md.ct_orig_tuple.ipv4.ipv4_proto;
    } else {
        ctx.key.src.addr.ipv6_aligned = pkt->md.ct_orig_tuple.ipv6.ipv6_src;
        ctx.key.dst.addr.ipv6_aligned = pkt->md.ct_orig_tuple.ipv6.ipv6_dst;

        if (ctx_in->key.nw_proto == IPPROTO_ICMPV6) {
            ctx.key.src.icmp_id = ctx_in->key.src.icmp_id;
            ctx.key.dst.icmp_id = ctx_in->key.dst.icmp_id;
            uint16_t src_port = ntohs(pkt->md.ct_orig_tuple.ipv6.src_port);
            ctx.key.src.icmp_type = (uint8_t) src_port;
            ctx.key.dst.icmp_type = reverse_icmp6_type(ctx.key.src.icmp_type);
        } else {
            ctx.key.src.port = pkt->md.ct_orig_tuple.ipv6.src_port;
            ctx.key.dst.port = pkt->md.ct_orig_tuple.ipv6.dst_port;
        }
        ctx.key.nw_proto = pkt->md.ct_orig_tuple.ipv6.ipv6_proto;
    }

    ctx.key.dl_type = ctx_in->key.dl_type;
    ctx.key.zone = pkt->md.ct_zone;
    ctx.hash = conn_key_hash(&ctx.key, ct->hash_basis);
    *bucket = hash_to_bucket(ctx.hash);
    ct_lock_lock(&ct->buckets[*bucket].lock);
    conn_key_lookup(&ct->buckets[*bucket], &ctx, now);
    *conn = ctx.conn;
    return *conn ? true : false;
}