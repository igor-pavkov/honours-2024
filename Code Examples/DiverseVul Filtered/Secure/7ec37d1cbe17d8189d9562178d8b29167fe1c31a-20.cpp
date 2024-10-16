static bool is_xmm_fast_hypercall(struct kvm_hv_hcall *hc)
{
	switch (hc->code) {
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST:
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE:
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST_EX:
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE_EX:
	case HVCALL_SEND_IPI_EX:
		return true;
	}

	return false;
}