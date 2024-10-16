void kvm_arch_hardware_enable(void *garbage)
{
	kvm_x86_ops->hardware_enable(garbage);
}