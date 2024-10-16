static enum ib_gid_type cma_route_gid_type(enum rdma_network_type network_type,
					   unsigned long supported_gids,
					   enum ib_gid_type default_gid)
{
	if ((network_type == RDMA_NETWORK_IPV4 ||
	     network_type == RDMA_NETWORK_IPV6) &&
	    test_bit(IB_GID_TYPE_ROCE_UDP_ENCAP, &supported_gids))
		return IB_GID_TYPE_ROCE_UDP_ENCAP;

	return default_gid;
}