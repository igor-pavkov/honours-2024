static int cma_igmp_send(struct net_device *ndev, union ib_gid *mgid, bool join)
{
	struct in_device *in_dev = NULL;

	if (ndev) {
		rtnl_lock();
		in_dev = __in_dev_get_rtnl(ndev);
		if (in_dev) {
			if (join)
				ip_mc_inc_group(in_dev,
						*(__be32 *)(mgid->raw + 12));
			else
				ip_mc_dec_group(in_dev,
						*(__be32 *)(mgid->raw + 12));
		}
		rtnl_unlock();
	}
	return (in_dev) ? 0 : -ENODEV;
}