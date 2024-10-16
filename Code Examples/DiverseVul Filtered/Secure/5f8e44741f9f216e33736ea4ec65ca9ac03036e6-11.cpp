static int nlmsg_populate_fdb_fill(struct sk_buff *skb,
				   struct net_device *dev,
				   u8 *addr, u16 vid, u32 pid, u32 seq,
				   int type, unsigned int flags,
				   int nlflags, u16 ndm_state)
{
	struct nlmsghdr *nlh;
	struct ndmsg *ndm;

	nlh = nlmsg_put(skb, pid, seq, type, sizeof(*ndm), nlflags);
	if (!nlh)
		return -EMSGSIZE;

	ndm = nlmsg_data(nlh);
	ndm->ndm_family  = AF_BRIDGE;
	ndm->ndm_pad1	 = 0;
	ndm->ndm_pad2    = 0;
	ndm->ndm_flags	 = flags;
	ndm->ndm_type	 = 0;
	ndm->ndm_ifindex = dev->ifindex;
	ndm->ndm_state   = ndm_state;

	if (nla_put(skb, NDA_LLADDR, ETH_ALEN, addr))
		goto nla_put_failure;
	if (vid)
		if (nla_put(skb, NDA_VLAN, sizeof(u16), &vid))
			goto nla_put_failure;

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}