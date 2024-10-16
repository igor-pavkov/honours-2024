void kvm_arch_hardware_unsetup(void)
{
	kvm_x86_ops->hardware_unsetup();
}