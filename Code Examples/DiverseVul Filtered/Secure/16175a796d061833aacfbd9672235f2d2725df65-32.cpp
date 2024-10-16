static __init int cpu_has_kvm_support(void)
{
	return cpu_has_vmx();
}