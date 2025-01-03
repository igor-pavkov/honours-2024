static int rtnl_fill_ifinfo(struct sk_buff *skb, struct net_device *dev,
			    int type, u32 pid, u32 seq, u32 change,
			    unsigned int flags, u32 ext_filter_mask)
{
	struct ifinfomsg *ifm;
	struct nlmsghdr *nlh;
	struct rtnl_link_stats64 temp;
	const struct rtnl_link_stats64 *stats;
	struct nlattr *attr, *af_spec;
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
#ifdef CONFIG_RPS
	    nla_put_u32(skb, IFLA_NUM_RX_QUEUES, dev->num_rx_queues) ||
#endif
	    (dev->ifindex != dev->iflink &&
	     nla_put_u32(skb, IFLA_LINK, dev->iflink)) ||
	    (upper_dev &&
	     nla_put_u32(skb, IFLA_MASTER, upper_dev->ifindex)) ||
	    nla_put_u8(skb, IFLA_CARRIER, netif_carrier_ok(dev)) ||
	    (dev->qdisc &&
	     nla_put_string(skb, IFLA_QDISC, dev->qdisc->ops->id)) ||
	    (dev->ifalias &&
	     nla_put_string(skb, IFLA_IFALIAS, dev->ifalias)))
		goto nla_put_failure;

	if (1) {
		struct rtnl_link_ifmap map = {
			.mem_start   = dev->mem_start,
			.mem_end     = dev->mem_end,
			.base_addr   = dev->base_addr,
			.irq         = dev->irq,
			.dma         = dev->dma,
			.port        = dev->if_port,
		};
		if (nla_put(skb, IFLA_MAP, sizeof(map), &map))
			goto nla_put_failure;
	}

	if (dev->addr_len) {
		if (nla_put(skb, IFLA_ADDRESS, dev->addr_len, dev->dev_addr) ||
		    nla_put(skb, IFLA_BROADCAST, dev->addr_len, dev->broadcast))
			goto nla_put_failure;
	}

	attr = nla_reserve(skb, IFLA_STATS,
			sizeof(struct rtnl_link_stats));
	if (attr == NULL)
		goto nla_put_failure;

	stats = dev_get_stats(dev, &temp);
	copy_rtnl_link_stats(nla_data(attr), stats);

	attr = nla_reserve(skb, IFLA_STATS64,
			sizeof(struct rtnl_link_stats64));
	if (attr == NULL)
		goto nla_put_failure;
	copy_rtnl_link_stats64(nla_data(attr), stats);

	if (dev->dev.parent && (ext_filter_mask & RTEXT_FILTER_VF) &&
	    nla_put_u32(skb, IFLA_NUM_VF, dev_num_vf(dev->dev.parent)))
		goto nla_put_failure;

	if (dev->netdev_ops->ndo_get_vf_config && dev->dev.parent
	    && (ext_filter_mask & RTEXT_FILTER_VF)) {
		int i;

		struct nlattr *vfinfo, *vf;
		int num_vfs = dev_num_vf(dev->dev.parent);

		vfinfo = nla_nest_start(skb, IFLA_VFINFO_LIST);
		if (!vfinfo)
			goto nla_put_failure;
		for (i = 0; i < num_vfs; i++) {
			struct ifla_vf_info ivi;
			struct ifla_vf_mac vf_mac;
			struct ifla_vf_vlan vf_vlan;
			struct ifla_vf_tx_rate vf_tx_rate;
			struct ifla_vf_spoofchk vf_spoofchk;

			/*
			 * Not all SR-IOV capable drivers support the
			 * spoofcheck query.  Preset to -1 so the user
			 * space tool can detect that the driver didn't
			 * report anything.
			 */
			ivi.spoofchk = -1;
			memset(ivi.mac, 0, sizeof(ivi.mac));
			if (dev->netdev_ops->ndo_get_vf_config(dev, i, &ivi))
				break;
			vf_mac.vf =
				vf_vlan.vf =
				vf_tx_rate.vf =
				vf_spoofchk.vf = ivi.vf;

			memcpy(vf_mac.mac, ivi.mac, sizeof(ivi.mac));
			vf_vlan.vlan = ivi.vlan;
			vf_vlan.qos = ivi.qos;
			vf_tx_rate.rate = ivi.tx_rate;
			vf_spoofchk.setting = ivi.spoofchk;
			vf = nla_nest_start(skb, IFLA_VF_INFO);
			if (!vf) {
				nla_nest_cancel(skb, vfinfo);
				goto nla_put_failure;
			}
			if (nla_put(skb, IFLA_VF_MAC, sizeof(vf_mac), &vf_mac) ||
			    nla_put(skb, IFLA_VF_VLAN, sizeof(vf_vlan), &vf_vlan) ||
			    nla_put(skb, IFLA_VF_TX_RATE, sizeof(vf_tx_rate),
				    &vf_tx_rate) ||
			    nla_put(skb, IFLA_VF_SPOOFCHK, sizeof(vf_spoofchk),
				    &vf_spoofchk))
				goto nla_put_failure;
			nla_nest_end(skb, vf);
		}
		nla_nest_end(skb, vfinfo);
	}

	if (rtnl_port_fill(skb, dev))
		goto nla_put_failure;

	if (dev->rtnl_link_ops) {
		if (rtnl_link_fill(skb, dev) < 0)
			goto nla_put_failure;
	}

	if (!(af_spec = nla_nest_start(skb, IFLA_AF_SPEC)))
		goto nla_put_failure;

	list_for_each_entry(af_ops, &rtnl_af_ops, list) {
		if (af_ops->fill_link_af) {
			struct nlattr *af;
			int err;

			if (!(af = nla_nest_start(skb, af_ops->family)))
				goto nla_put_failure;

			err = af_ops->fill_link_af(skb, dev);

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

	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}