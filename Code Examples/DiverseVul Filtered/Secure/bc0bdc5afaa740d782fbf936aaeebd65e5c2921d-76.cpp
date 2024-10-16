int cma_get_default_roce_tos(struct cma_device *cma_dev, u32 port)
{
	if (!rdma_is_port_valid(cma_dev->device, port))
		return -EINVAL;

	return cma_dev->default_roce_tos[port - rdma_start_port(cma_dev->device)];
}