static void hardware_disable(void *garbage)
{
	vmclear_local_vcpus();
	kvm_cpu_vmxoff();
}