static int nested_vmx_check_msr_switch(struct kvm_vcpu *vcpu,
				       u32 count, u64 addr)
{
	int maxphyaddr;

	if (count == 0)
		return 0;
	maxphyaddr = cpuid_maxphyaddr(vcpu);
	if (!IS_ALIGNED(addr, 16) || addr >> maxphyaddr ||
	    (addr + count * sizeof(struct vmx_msr_entry) - 1) >> maxphyaddr)
		return -EINVAL;

	return 0;
}