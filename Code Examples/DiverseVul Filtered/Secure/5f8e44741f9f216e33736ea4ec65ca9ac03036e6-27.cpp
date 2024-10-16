static int do_setvfinfo(struct net_device *dev, struct nlattr **tb)
{
	const struct net_device_ops *ops = dev->netdev_ops;
	int err = -EINVAL;

	if (tb[IFLA_VF_MAC]) {
		struct ifla_vf_mac *ivm = nla_data(tb[IFLA_VF_MAC]);

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_mac)
			err = ops->ndo_set_vf_mac(dev, ivm->vf,
						  ivm->mac);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_VLAN]) {
		struct ifla_vf_vlan *ivv = nla_data(tb[IFLA_VF_VLAN]);

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_vlan)
			err = ops->ndo_set_vf_vlan(dev, ivv->vf, ivv->vlan,
						   ivv->qos);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_TX_RATE]) {
		struct ifla_vf_tx_rate *ivt = nla_data(tb[IFLA_VF_TX_RATE]);
		struct ifla_vf_info ivf;

		err = -EOPNOTSUPP;
		if (ops->ndo_get_vf_config)
			err = ops->ndo_get_vf_config(dev, ivt->vf, &ivf);
		if (err < 0)
			return err;

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_rate)
			err = ops->ndo_set_vf_rate(dev, ivt->vf,
						   ivf.min_tx_rate,
						   ivt->rate);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_RATE]) {
		struct ifla_vf_rate *ivt = nla_data(tb[IFLA_VF_RATE]);

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_rate)
			err = ops->ndo_set_vf_rate(dev, ivt->vf,
						   ivt->min_tx_rate,
						   ivt->max_tx_rate);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_SPOOFCHK]) {
		struct ifla_vf_spoofchk *ivs = nla_data(tb[IFLA_VF_SPOOFCHK]);

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_spoofchk)
			err = ops->ndo_set_vf_spoofchk(dev, ivs->vf,
						       ivs->setting);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_LINK_STATE]) {
		struct ifla_vf_link_state *ivl = nla_data(tb[IFLA_VF_LINK_STATE]);

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_link_state)
			err = ops->ndo_set_vf_link_state(dev, ivl->vf,
							 ivl->link_state);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_RSS_QUERY_EN]) {
		struct ifla_vf_rss_query_en *ivrssq_en;

		err = -EOPNOTSUPP;
		ivrssq_en = nla_data(tb[IFLA_VF_RSS_QUERY_EN]);
		if (ops->ndo_set_vf_rss_query_en)
			err = ops->ndo_set_vf_rss_query_en(dev, ivrssq_en->vf,
							   ivrssq_en->setting);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_TRUST]) {
		struct ifla_vf_trust *ivt = nla_data(tb[IFLA_VF_TRUST]);

		err = -EOPNOTSUPP;
		if (ops->ndo_set_vf_trust)
			err = ops->ndo_set_vf_trust(dev, ivt->vf, ivt->setting);
		if (err < 0)
			return err;
	}

	if (tb[IFLA_VF_IB_NODE_GUID]) {
		struct ifla_vf_guid *ivt = nla_data(tb[IFLA_VF_IB_NODE_GUID]);

		if (!ops->ndo_set_vf_guid)
			return -EOPNOTSUPP;

		return handle_vf_guid(dev, ivt, IFLA_VF_IB_NODE_GUID);
	}

	if (tb[IFLA_VF_IB_PORT_GUID]) {
		struct ifla_vf_guid *ivt = nla_data(tb[IFLA_VF_IB_PORT_GUID]);

		if (!ops->ndo_set_vf_guid)
			return -EOPNOTSUPP;

		return handle_vf_guid(dev, ivt, IFLA_VF_IB_PORT_GUID);
	}

	return err;
}