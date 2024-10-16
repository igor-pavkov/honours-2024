static int xfrm_set_spdinfo(struct sk_buff *skb, struct nlmsghdr *nlh,
			    struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrmu_spdhthresh *thresh4 = NULL;
	struct xfrmu_spdhthresh *thresh6 = NULL;

	/* selector prefixlen thresholds to hash policies */
	if (attrs[XFRMA_SPD_IPV4_HTHRESH]) {
		struct nlattr *rta = attrs[XFRMA_SPD_IPV4_HTHRESH];

		if (nla_len(rta) < sizeof(*thresh4))
			return -EINVAL;
		thresh4 = nla_data(rta);
		if (thresh4->lbits > 32 || thresh4->rbits > 32)
			return -EINVAL;
	}
	if (attrs[XFRMA_SPD_IPV6_HTHRESH]) {
		struct nlattr *rta = attrs[XFRMA_SPD_IPV6_HTHRESH];

		if (nla_len(rta) < sizeof(*thresh6))
			return -EINVAL;
		thresh6 = nla_data(rta);
		if (thresh6->lbits > 128 || thresh6->rbits > 128)
			return -EINVAL;
	}

	if (thresh4 || thresh6) {
		write_seqlock(&net->xfrm.policy_hthresh.lock);
		if (thresh4) {
			net->xfrm.policy_hthresh.lbits4 = thresh4->lbits;
			net->xfrm.policy_hthresh.rbits4 = thresh4->rbits;
		}
		if (thresh6) {
			net->xfrm.policy_hthresh.lbits6 = thresh6->lbits;
			net->xfrm.policy_hthresh.rbits6 = thresh6->rbits;
		}
		write_sequnlock(&net->xfrm.policy_hthresh.lock);

		xfrm_policy_hash_rebuild(net);
	}

	return 0;
}