static enum rdma_ucm_port_space rdma_ps_from_service_id(__be64 service_id)
{
	return (be64_to_cpu(service_id) >> 16) & 0xffff;
}