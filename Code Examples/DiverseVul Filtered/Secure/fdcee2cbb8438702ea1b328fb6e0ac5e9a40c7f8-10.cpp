static void sctp_v6_err(struct sk_buff *skb, struct inet6_skb_parm *opt,
			u8 type, u8 code, int offset, __be32 info)
{
	struct inet6_dev *idev;
	struct sock *sk;
	struct sctp_association *asoc;
	struct sctp_transport *transport;
	struct ipv6_pinfo *np;
	__u16 saveip, savesctp;
	int err;
	struct net *net = dev_net(skb->dev);

	idev = in6_dev_get(skb->dev);

	/* Fix up skb to look at the embedded net header. */
	saveip	 = skb->network_header;
	savesctp = skb->transport_header;
	skb_reset_network_header(skb);
	skb_set_transport_header(skb, offset);
	sk = sctp_err_lookup(net, AF_INET6, skb, sctp_hdr(skb), &asoc, &transport);
	/* Put back, the original pointers. */
	skb->network_header   = saveip;
	skb->transport_header = savesctp;
	if (!sk) {
		__ICMP6_INC_STATS(net, idev, ICMP6_MIB_INERRORS);
		goto out;
	}

	/* Warning:  The sock lock is held.  Remember to call
	 * sctp_err_finish!
	 */

	switch (type) {
	case ICMPV6_PKT_TOOBIG:
		if (ip6_sk_accept_pmtu(sk))
			sctp_icmp_frag_needed(sk, asoc, transport, ntohl(info));
		goto out_unlock;
	case ICMPV6_PARAMPROB:
		if (ICMPV6_UNK_NEXTHDR == code) {
			sctp_icmp_proto_unreachable(sk, asoc, transport);
			goto out_unlock;
		}
		break;
	case NDISC_REDIRECT:
		sctp_icmp_redirect(sk, transport, skb);
		goto out_unlock;
	default:
		break;
	}

	np = inet6_sk(sk);
	icmpv6_err_convert(type, code, &err);
	if (!sock_owned_by_user(sk) && np->recverr) {
		sk->sk_err = err;
		sk->sk_error_report(sk);
	} else {  /* Only an error on timeout */
		sk->sk_err_soft = err;
	}

out_unlock:
	sctp_err_finish(sk, transport);
out:
	if (likely(idev != NULL))
		in6_dev_put(idev);
}