void kvm_arch_check_processor_compat(void *rtn)
{
	kvm_x86_ops->check_processor_compatibility(rtn);
}