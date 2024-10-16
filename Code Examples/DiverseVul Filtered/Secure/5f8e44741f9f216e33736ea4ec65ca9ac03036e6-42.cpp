static inline int rtnl_vfinfo_size(const struct net_device *dev,
				   u32 ext_filter_mask)
{
	if (dev->dev.parent && dev_is_pci(dev->dev.parent) &&
	    (ext_filter_mask & RTEXT_FILTER_VF)) {
		int num_vfs = dev_num_vf(dev->dev.parent);
		size_t size = nla_total_size(sizeof(struct nlattr));
		size += nla_total_size(num_vfs * sizeof(struct nlattr));
		size += num_vfs *
			(nla_total_size(sizeof(struct ifla_vf_mac)) +
			 nla_total_size(sizeof(struct ifla_vf_vlan)) +
			 nla_total_size(sizeof(struct ifla_vf_spoofchk)) +
			 nla_total_size(sizeof(struct ifla_vf_rate)) +
			 nla_total_size(sizeof(struct ifla_vf_link_state)) +
			 nla_total_size(sizeof(struct ifla_vf_rss_query_en)) +
			 /* IFLA_VF_STATS_RX_PACKETS */
			 nla_total_size(sizeof(__u64)) +
			 /* IFLA_VF_STATS_TX_PACKETS */
			 nla_total_size(sizeof(__u64)) +
			 /* IFLA_VF_STATS_RX_BYTES */
			 nla_total_size(sizeof(__u64)) +
			 /* IFLA_VF_STATS_TX_BYTES */
			 nla_total_size(sizeof(__u64)) +
			 /* IFLA_VF_STATS_BROADCAST */
			 nla_total_size(sizeof(__u64)) +
			 /* IFLA_VF_STATS_MULTICAST */
			 nla_total_size(sizeof(__u64)) +
			 nla_total_size(sizeof(struct ifla_vf_trust)));
		return size;
	} else
		return 0;
}