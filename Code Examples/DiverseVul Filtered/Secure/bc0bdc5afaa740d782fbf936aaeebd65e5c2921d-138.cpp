static u16 cma_port_from_service_id(__be64 service_id)
{
	return (u16)be64_to_cpu(service_id);
}