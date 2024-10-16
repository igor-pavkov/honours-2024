int ndo_dflt_bridge_getlink(struct sk_buff *skb, u32 pid, u32 seq,
			    struct net_device *dev, u16 mode,
			    u32 flags, u32 mask, int nlflags,
			    u32 filter_mask,
			    int (*vlan_fill)(struct sk_buff *skb,
					     struct net_device *dev,
					     u32 filter_mask))
{
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;
	struct nlattr *br_afspec;
	struct nlattr *protinfo;
	u8 operstate = netif_running(dev) ? dev->operstate : IF_OPER_DOWN;
	struct net_device *br_dev = netdev_master_upper_dev_get(dev);
	int err = 0;

	nlh = nlmsg_put(skb, pid, seq, RTM_NEWLINK, sizeof(*ifm), nlflags);
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
	    (dev->ifindex != dev_get_iflink(dev) &&
	     nla_put_u32(skb, IFLA_LINK, dev_get_iflink(dev))))
		goto nla_put_failure;

	br_afspec = nla_nest_start(skb, IFLA_AF_SPEC);
	if (!br_afspec)
		goto nla_put_failure;

	if (nla_put_u16(skb, IFLA_BRIDGE_FLAGS, BRIDGE_FLAGS_SELF)) {
		nla_nest_cancel(skb, br_afspec);
		goto nla_put_failure;
	}

	if (mode != BRIDGE_MODE_UNDEF) {
		if (nla_put_u16(skb, IFLA_BRIDGE_MODE, mode)) {
			nla_nest_cancel(skb, br_afspec);
			goto nla_put_failure;
		}
	}
	if (vlan_fill) {
		err = vlan_fill(skb, dev, filter_mask);
		if (err) {
			nla_nest_cancel(skb, br_afspec);
			goto nla_put_failure;
		}
	}
	nla_nest_end(skb, br_afspec);

	protinfo = nla_nest_start(skb, IFLA_PROTINFO | NLA_F_NESTED);
	if (!protinfo)
		goto nla_put_failure;

	if (brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_MODE, BR_HAIRPIN_MODE) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_GUARD, BR_BPDU_GUARD) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_FAST_LEAVE,
				BR_MULTICAST_FAST_LEAVE) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_PROTECT, BR_ROOT_BLOCK) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_LEARNING, BR_LEARNING) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_LEARNING_SYNC, BR_LEARNING_SYNC) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_UNICAST_FLOOD, BR_FLOOD) ||
	    brport_nla_put_flag(skb, flags, mask,
				IFLA_BRPORT_PROXYARP, BR_PROXYARP)) {
		nla_nest_cancel(skb, protinfo);
		goto nla_put_failure;
	}

	nla_nest_end(skb, protinfo);

	nlmsg_end(skb, nlh);
	return 0;
nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return err ? err : -EMSGSIZE;
}