int ndo_dflt_bridge_getlink(struct sk_buff *skb, u32 pid, u32 seq,
			    struct net_device *dev, u16 mode)
{
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;
	struct nlattr *br_afspec;
	u8 operstate = netif_running(dev) ? dev->operstate : IF_OPER_DOWN;
	struct net_device *br_dev = netdev_master_upper_dev_get(dev);

	nlh = nlmsg_put(skb, pid, seq, RTM_NEWLINK, sizeof(*ifm), NLM_F_MULTI);
	if (nlh == NULL)
		return -EMSGSIZE;

	ifm = nlmsg_data(nlh);
	ifm->ifi_family = AF_BRIDGE;
	ifm->__ifi_pad = 0;
	ifm->ifi_type = dev->type;
	ifm->ifi_index = dev->ifindex;
	ifm->ifi_flags = dev_get_flags(dev);
	ifm->ifi_change = 0;


	if (nla_put_string(skb, IFLA_IFNAME, dev->name) ||
	    nla_put_u32(skb, IFLA_MTU, dev->mtu) ||
	    nla_put_u8(skb, IFLA_OPERSTATE, operstate) ||
	    (br_dev &&
	     nla_put_u32(skb, IFLA_MASTER, br_dev->ifindex)) ||
	    (dev->addr_len &&
	     nla_put(skb, IFLA_ADDRESS, dev->addr_len, dev->dev_addr)) ||
	    (dev->ifindex != dev->iflink &&
	     nla_put_u32(skb, IFLA_LINK, dev->iflink)))
		goto nla_put_failure;

	br_afspec = nla_nest_start(skb, IFLA_AF_SPEC);
	if (!br_afspec)
		goto nla_put_failure;

	if (nla_put_u16(skb, IFLA_BRIDGE_FLAGS, BRIDGE_FLAGS_SELF) ||
	    nla_put_u16(skb, IFLA_BRIDGE_MODE, mode)) {
		nla_nest_cancel(skb, br_afspec);
		goto nla_put_failure;
	}
	nla_nest_end(skb, br_afspec);

	return nlmsg_end(skb, nlh);
nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}