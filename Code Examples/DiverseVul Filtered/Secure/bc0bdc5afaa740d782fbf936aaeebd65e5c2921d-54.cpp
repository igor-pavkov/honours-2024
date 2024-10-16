__be64 rdma_get_service_id(struct rdma_cm_id *id, struct sockaddr *addr)
{
	if (addr->sa_family == AF_IB)
		return ((struct sockaddr_ib *) addr)->sib_sid;

	return cpu_to_be64(((u64)id->ps << 16) + be16_to_cpu(cma_port(addr)));
}