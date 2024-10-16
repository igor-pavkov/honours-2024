static int xfrm6_tunnel_err(struct sk_buff *skb, struct inet6_skb_parm *opt,
			    int type, int code, int offset, __be32 info)
{
	/* xfrm6_tunnel native err handling */
	switch (type) {
	case ICMPV6_DEST_UNREACH:
		switch (code) {
		case ICMPV6_NOROUTE:
		case ICMPV6_ADM_PROHIBITED:
		case ICMPV6_NOT_NEIGHBOUR:
		case ICMPV6_ADDR_UNREACH:
		case ICMPV6_PORT_UNREACH:
		default:
			break;
		}
		break;
	case ICMPV6_PKT_TOOBIG:
		break;
	case ICMPV6_TIME_EXCEED:
		switch (code) {
		case ICMPV6_EXC_HOPLIMIT:
			break;
		case ICMPV6_EXC_FRAGTIME:
		default:
			break;
		}
		break;
	case ICMPV6_PARAMPROB:
		switch (code) {
		case ICMPV6_HDR_FIELD: break;
		case ICMPV6_UNK_NEXTHDR: break;
		case ICMPV6_UNK_OPTION: break;
		}
		break;
	default:
		break;
	}

	return 0;
}