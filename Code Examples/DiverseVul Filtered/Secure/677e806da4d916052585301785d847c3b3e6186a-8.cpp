static int build_spdinfo(struct sk_buff *skb, struct net *net,
			 u32 portid, u32 seq, u32 flags)
{
	struct xfrmk_spdinfo si;
	struct xfrmu_spdinfo spc;
	struct xfrmu_spdhinfo sph;
	struct xfrmu_spdhthresh spt4, spt6;
	struct nlmsghdr *nlh;
	int err;
	u32 *f;
	unsigned lseq;

	nlh = nlmsg_put(skb, portid, seq, XFRM_MSG_NEWSPDINFO, sizeof(u32), 0);
	if (nlh == NULL) /* shouldn't really happen ... */
		return -EMSGSIZE;

	f = nlmsg_data(nlh);
	*f = flags;
	xfrm_spd_getinfo(net, &si);
	spc.incnt = si.incnt;
	spc.outcnt = si.outcnt;
	spc.fwdcnt = si.fwdcnt;
	spc.inscnt = si.inscnt;
	spc.outscnt = si.outscnt;
	spc.fwdscnt = si.fwdscnt;
	sph.spdhcnt = si.spdhcnt;
	sph.spdhmcnt = si.spdhmcnt;

	do {
		lseq = read_seqbegin(&net->xfrm.policy_hthresh.lock);

		spt4.lbits = net->xfrm.policy_hthresh.lbits4;
		spt4.rbits = net->xfrm.policy_hthresh.rbits4;
		spt6.lbits = net->xfrm.policy_hthresh.lbits6;
		spt6.rbits = net->xfrm.policy_hthresh.rbits6;
	} while (read_seqretry(&net->xfrm.policy_hthresh.lock, lseq));

	err = nla_put(skb, XFRMA_SPD_INFO, sizeof(spc), &spc);
	if (!err)
		err = nla_put(skb, XFRMA_SPD_HINFO, sizeof(sph), &sph);
	if (!err)
		err = nla_put(skb, XFRMA_SPD_IPV4_HTHRESH, sizeof(spt4), &spt4);
	if (!err)
		err = nla_put(skb, XFRMA_SPD_IPV6_HTHRESH, sizeof(spt6), &spt6);
	if (err) {
		nlmsg_cancel(skb, nlh);
		return err;
	}

	nlmsg_end(skb, nlh);
	return 0;
}