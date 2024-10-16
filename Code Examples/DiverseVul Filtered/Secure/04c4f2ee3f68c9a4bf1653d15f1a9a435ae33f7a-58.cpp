static void hardware_unsetup(void)
{
	if (nested)
		nested_vmx_hardware_unsetup();

	free_kvm_area();
}