static void __vcpu_clear(void *arg)
{
	struct vcpu_vmx *vmx = arg;
	int cpu = raw_smp_processor_id();

	if (vmx->vcpu.cpu == cpu)
		vmcs_clear(vmx->vmcs);
	if (per_cpu(current_vmcs, cpu) == vmx->vmcs)
		per_cpu(current_vmcs, cpu) = NULL;
	rdtscll(vmx->vcpu.arch.host_tsc);
	list_del(&vmx->local_vcpus_link);
	vmx->vcpu.cpu = -1;
	vmx->launched = 0;
}