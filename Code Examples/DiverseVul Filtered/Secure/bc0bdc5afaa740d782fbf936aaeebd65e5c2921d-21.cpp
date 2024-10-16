int cma_set_default_gid_type(struct cma_device *cma_dev,
			     u32 port,
			     enum ib_gid_type default_gid_type)
{
	unsigned long supported_gids;

	if (!rdma_is_port_valid(cma_dev->device, port))
		return -EINVAL;

	if (default_gid_type == IB_GID_TYPE_IB &&
	    rdma_protocol_roce_eth_encap(cma_dev->device, port))
		default_gid_type = IB_GID_TYPE_ROCE;

	supported_gids = roce_gid_type_mask_support(cma_dev->device, port);

	if (!(supported_gids & 1 << default_gid_type))
		return -EINVAL;

	cma_dev->default_gid_type[port - rdma_start_port(cma_dev->device)] =
		default_gid_type;

	return 0;
}