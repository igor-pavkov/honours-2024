static void hardware_disable(void)
{
	vmclear_local_loaded_vmcss();

	if (cpu_vmxoff())
		kvm_spurious_fault();

	intel_pt_handle_vmx(0);
}