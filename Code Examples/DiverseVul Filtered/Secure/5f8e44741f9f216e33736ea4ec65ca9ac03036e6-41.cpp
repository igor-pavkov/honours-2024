static int handle_infiniband_guid(struct net_device *dev, struct ifla_vf_guid *ivt,
				  int guid_type)
{
	const struct net_device_ops *ops = dev->netdev_ops;

	return ops->ndo_set_vf_guid(dev, ivt->vf, ivt->guid, guid_type);
}