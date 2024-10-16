static int build_spdinfo(struct sk_buff *skb, struct net *net,
			 u32 pid, u32 seq, u32 flags)
{
	struct xfrmk_spdinfo si;
	struct xfrmu_spdinfo spc;
	struct xfrmu_spdhinfo sph;
	struct nlmsghdr *nlh;
	int err;
	u32 *f;

	nlh = nlmsg_put(skb, pid, seq, XFRM_MSG_NEWSPDINFO, sizeof(u32), 0);
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

	err = nla_put(skb, XFRMA_SPD_INFO, sizeof(spc), &spc);
	if (!err)
		err = nla_put(skb, XFRMA_SPD_HINFO, sizeof(sph), &sph);
	if (err) {
		nlmsg_cancel(skb, nlh);
		return err;
	}

	return nlmsg_end(skb, nlh);
}