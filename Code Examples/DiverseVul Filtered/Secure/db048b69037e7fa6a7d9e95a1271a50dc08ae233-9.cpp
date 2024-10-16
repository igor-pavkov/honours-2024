void ethtool_ntuple_flush(struct net_device *dev)
{
	struct ethtool_rx_ntuple_flow_spec_container *fsc, *f;

	list_for_each_entry_safe(fsc, f, &dev->ethtool_ntuple_list.list, list) {
		list_del(&fsc->list);
		kfree(fsc);
	}
	dev->ethtool_ntuple_list.count = 0;
}