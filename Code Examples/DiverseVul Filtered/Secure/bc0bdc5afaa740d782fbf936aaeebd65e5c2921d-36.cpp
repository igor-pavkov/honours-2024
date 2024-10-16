int cma_set_default_roce_tos(struct cma_device *cma_dev, u32 port,
			     u8 default_roce_tos)
{
	if (!rdma_is_port_valid(cma_dev->device, port))
		return -EINVAL;

	cma_dev->default_roce_tos[port - rdma_start_port(cma_dev->device)] =
		 default_roce_tos;

	return 0;
}