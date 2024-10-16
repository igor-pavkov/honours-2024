static int rtnl_fill_ifinfo(struct sk_buff *skb, struct net_device *dev,
			    int type, u32 pid, u32 seq, u32 change,
			    unsigned int flags, u32 ext_filter_mask)
{
	struct ifinfomsg *ifm;
	struct nlmsghdr *nlh;
	struct nlattr *af_spec;
	struct rtnl_af_ops *af_ops;
	struct net_device *upper_dev = netdev_master_upper_dev_get(dev);

	ASSERT_RTNL();
	nlh = nlmsg_put(skb, pid, seq, type, sizeof(*ifm), flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	ifm = nlmsg_data(nlh);
	ifm->ifi_family = AF_UNSPEC;
	ifm->__ifi_pad = 0;
	ifm->ifi_type = dev->type;
	ifm->ifi_index = dev->ifindex;
	ifm->ifi_flags = dev_get_flags(dev);
	ifm->ifi_change = change;

	if (nla_put_string(skb, IFLA_IFNAME, dev->name) ||
	    nla_put_u32(skb, IFLA_TXQLEN, dev->tx_queue_len) ||
	    nla_put_u8(skb, IFLA_OPERSTATE,
		       netif_running(dev) ? dev->operstate : IF_OPER_DOWN) ||
	    nla_put_u8(skb, IFLA_LINKMODE, dev->link_mode) ||
	    nla_put_u32(skb, IFLA_MTU, dev->mtu) ||
	    nla_put_u32(skb, IFLA_GROUP, dev->group) ||
	    nla_put_u32(skb, IFLA_PROMISCUITY, dev->promiscuity) ||
	    nla_put_u32(skb, IFLA_NUM_TX_QUEUES, dev->num_tx_queues) ||
	    nla_put_u32(skb, IFLA_GSO_MAX_SEGS, dev->gso_max_segs) ||
	    nla_put_u32(skb, IFLA_GSO_MAX_SIZE, dev->gso_max_size) ||
#ifdef CONFIG_RPS
	    nla_put_u32(skb, IFLA_NUM_RX_QUEUES, dev->num_rx_queues) ||
#endif
	    (dev->ifindex != dev_get_iflink(dev) &&
	     nla_put_u32(skb, IFLA_LINK, dev_get_iflink(dev))) ||
	    (upper_dev &&
	     nla_put_u32(skb, IFLA_MASTER, upper_dev->ifindex)) ||
	    nla_put_u8(skb, IFLA_CARRIER, netif_carrier_ok(dev)) ||
	    (dev->qdisc &&
	     nla_put_string(skb, IFLA_QDISC, dev->qdisc->ops->id)) ||
	    (dev->ifalias &&
	     nla_put_string(skb, IFLA_IFALIAS, dev->ifalias)) ||
	    nla_put_u32(skb, IFLA_CARRIER_CHANGES,
			atomic_read(&dev->carrier_changes)) ||
	    nla_put_u8(skb, IFLA_PROTO_DOWN, dev->proto_down))
		goto nla_put_failure;

	if (rtnl_fill_link_ifmap(skb, dev))
		goto nla_put_failure;

	if (dev->addr_len) {
		if (nla_put(skb, IFLA_ADDRESS, dev->addr_len, dev->dev_addr) ||
		    nla_put(skb, IFLA_BROADCAST, dev->addr_len, dev->broadcast))
			goto nla_put_failure;
	}

	if (rtnl_phys_port_id_fill(skb, dev))
		goto nla_put_failure;

	if (rtnl_phys_port_name_fill(skb, dev))
		goto nla_put_failure;

	if (rtnl_phys_switch_id_fill(skb, dev))
		goto nla_put_failure;

	if (rtnl_fill_stats(skb, dev))
		goto nla_put_failure;

	if (dev->dev.parent && (ext_filter_mask & RTEXT_FILTER_VF) &&
	    nla_put_u32(skb, IFLA_NUM_VF, dev_num_vf(dev->dev.parent)))
		goto nla_put_failure;

	if (dev->netdev_ops->ndo_get_vf_config && dev->dev.parent &&
	    ext_filter_mask & RTEXT_FILTER_VF) {
		int i;
		struct nlattr *vfinfo;
		int num_vfs = dev_num_vf(dev->dev.parent);

		vfinfo = nla_nest_start(skb, IFLA_VFINFO_LIST);
		if (!vfinfo)
			goto nla_put_failure;
		for (i = 0; i < num_vfs; i++) {
			if (rtnl_fill_vfinfo(skb, dev, i, vfinfo))
				goto nla_put_failure;
		}

		nla_nest_end(skb, vfinfo);
	}

	if (rtnl_port_fill(skb, dev, ext_filter_mask))
		goto nla_put_failure;

	if (dev->rtnl_link_ops || rtnl_have_link_slave_info(dev)) {
		if (rtnl_link_fill(skb, dev) < 0)
			goto nla_put_failure;
	}

	if (dev->rtnl_link_ops &&
	    dev->rtnl_link_ops->get_link_net) {
		struct net *link_net = dev->rtnl_link_ops->get_link_net(dev);

		if (!net_eq(dev_net(dev), link_net)) {
			int id = peernet2id_alloc(dev_net(dev), link_net);

			if (nla_put_s32(skb, IFLA_LINK_NETNSID, id))
				goto nla_put_failure;
		}
	}

	if (!(af_spec = nla_nest_start(skb, IFLA_AF_SPEC)))
		goto nla_put_failure;

	list_for_each_entry(af_ops, &rtnl_af_ops, list) {
		if (af_ops->fill_link_af) {
			struct nlattr *af;
			int err;

			if (!(af = nla_nest_start(skb, af_ops->family)))
				goto nla_put_failure;

			err = af_ops->fill_link_af(skb, dev, ext_filter_mask);

			/*
			 * Caller may return ENODATA to indicate that there
			 * was no data to be dumped. This is not an error, it
			 * means we should trim the attribute header and
			 * continue.
			 */
			if (err == -ENODATA)
				nla_nest_cancel(skb, af);
			else if (err < 0)
				goto nla_put_failure;

			nla_nest_end(skb, af);
		}
	}

	nla_nest_end(skb, af_spec);

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}