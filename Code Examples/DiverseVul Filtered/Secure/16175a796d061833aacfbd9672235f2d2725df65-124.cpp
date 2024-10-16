static void guest_write_tsc(u64 guest_tsc, u64 host_tsc)
{
	vmcs_write64(TSC_OFFSET, guest_tsc - host_tsc);
}