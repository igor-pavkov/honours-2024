static inline int DetectRunInspectRuleHeader(
    const Packet *p,
    const Flow *f,
    const Signature *s,
    const uint32_t sflags,
    const uint8_t s_proto_flags)
{
    /* check if this signature has a requirement for flowvars of some type
     * and if so, if we actually have any in the flow. If not, the sig
     * can't match and we skip it. */
    if ((p->flags & PKT_HAS_FLOW) && (sflags & SIG_FLAG_REQUIRE_FLOWVAR)) {
        DEBUG_VALIDATE_BUG_ON(f == NULL);

        int m  = f->flowvar ? 1 : 0;

        /* no flowvars? skip this sig */
        if (m == 0) {
            SCLogDebug("skipping sig as the flow has no flowvars and sig "
                    "has SIG_FLAG_REQUIRE_FLOWVAR flag set.");
            return 0;
        }
    }

    if ((s_proto_flags & DETECT_PROTO_IPV4) && !PKT_IS_IPV4(p)) {
        SCLogDebug("ip version didn't match");
        return 0;
    }
    if ((s_proto_flags & DETECT_PROTO_IPV6) && !PKT_IS_IPV6(p)) {
        SCLogDebug("ip version didn't match");
        return 0;
    }

    if (DetectProtoContainsProto(&s->proto, IP_GET_IPPROTO(p)) == 0) {
        SCLogDebug("proto didn't match");
        return 0;
    }

    /* check the source & dst port in the sig */
    if (p->proto == IPPROTO_TCP || p->proto == IPPROTO_UDP || p->proto == IPPROTO_SCTP) {
        if (!(sflags & SIG_FLAG_DP_ANY)) {
            if (p->flags & PKT_IS_FRAGMENT)
                return 0;
            DetectPort *dport = DetectPortLookupGroup(s->dp,p->dp);
            if (dport == NULL) {
                SCLogDebug("dport didn't match.");
                return 0;
            }
        }
        if (!(sflags & SIG_FLAG_SP_ANY)) {
            if (p->flags & PKT_IS_FRAGMENT)
                return 0;
            DetectPort *sport = DetectPortLookupGroup(s->sp,p->sp);
            if (sport == NULL) {
                SCLogDebug("sport didn't match.");
                return 0;
            }
        }
    } else if ((sflags & (SIG_FLAG_DP_ANY|SIG_FLAG_SP_ANY)) != (SIG_FLAG_DP_ANY|SIG_FLAG_SP_ANY)) {
        SCLogDebug("port-less protocol and sig needs ports");
        return 0;
    }

    /* check the destination address */
    if (!(sflags & SIG_FLAG_DST_ANY)) {
        if (PKT_IS_IPV4(p)) {
            if (DetectAddressMatchIPv4(s->addr_dst_match4, s->addr_dst_match4_cnt, &p->dst) == 0)
                return 0;
        } else if (PKT_IS_IPV6(p)) {
            if (DetectAddressMatchIPv6(s->addr_dst_match6, s->addr_dst_match6_cnt, &p->dst) == 0)
                return 0;
        }
    }
    /* check the source address */
    if (!(sflags & SIG_FLAG_SRC_ANY)) {
        if (PKT_IS_IPV4(p)) {
            if (DetectAddressMatchIPv4(s->addr_src_match4, s->addr_src_match4_cnt, &p->src) == 0)
                return 0;
        } else if (PKT_IS_IPV6(p)) {
            if (DetectAddressMatchIPv6(s->addr_src_match6, s->addr_src_match6_cnt, &p->src) == 0)
                return 0;
        }
    }

    return 1;
}