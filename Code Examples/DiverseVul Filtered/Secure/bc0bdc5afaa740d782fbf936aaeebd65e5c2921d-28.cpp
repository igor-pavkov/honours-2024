static bool cma_protocol_roce(const struct rdma_cm_id *id)
{
	struct ib_device *device = id->device;
	const u32 port_num = id->port_num ?: rdma_start_port(device);

	return rdma_protocol_roce(device, port_num);
}