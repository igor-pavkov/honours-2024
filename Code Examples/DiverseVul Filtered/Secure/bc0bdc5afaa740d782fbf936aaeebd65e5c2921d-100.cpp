static bool cma_match_net_dev(const struct rdma_cm_id *id,
			      const struct net_device *net_dev,
			      const struct cma_req_info *req)
{
	const struct rdma_addr *addr = &id->route.addr;

	if (!net_dev)
		/* This request is an AF_IB request */
		return (!id->port_num || id->port_num == req->port) &&
		       (addr->src_addr.ss_family == AF_IB);

	/*
	 * If the request is not for IPv6 link local, allow matching
	 * request to any netdevice of the one or multiport rdma device.
	 */
	if (!cma_is_req_ipv6_ll(req))
		return true;
	/*
	 * Net namespaces must match, and if the listner is listening
	 * on a specific netdevice than netdevice must match as well.
	 */
	if (net_eq(dev_net(net_dev), addr->dev_addr.net) &&
	    (!!addr->dev_addr.bound_dev_if ==
	     (addr->dev_addr.bound_dev_if == net_dev->ifindex)))
		return true;
	else
		return false;
}