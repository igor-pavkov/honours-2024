int cma_get_default_gid_type(struct cma_device *cma_dev,
			     u32 port)
{
	if (!rdma_is_port_valid(cma_dev->device, port))
		return -EINVAL;

	return cma_dev->default_gid_type[port - rdma_start_port(cma_dev->device)];
}