static bool cma_supported(struct ib_device *device)
{
	u32 i;

	rdma_for_each_port(device, i) {
		if (rdma_cap_ib_cm(device, i) || rdma_cap_iw_cm(device, i))
			return true;
	}
	return false;
}