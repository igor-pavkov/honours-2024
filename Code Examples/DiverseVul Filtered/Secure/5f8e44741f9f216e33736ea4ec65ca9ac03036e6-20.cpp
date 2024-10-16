static int handle_vf_guid(struct net_device *dev, struct ifla_vf_guid *ivt, int guid_type)
{
	if (dev->type != ARPHRD_INFINIBAND)
		return -EOPNOTSUPP;

	return handle_infiniband_guid(dev, ivt, guid_type);
}