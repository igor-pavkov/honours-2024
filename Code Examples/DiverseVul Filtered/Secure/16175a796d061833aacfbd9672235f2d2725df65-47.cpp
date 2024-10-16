static __exit void hardware_unsetup(void)
{
	free_kvm_area();
}