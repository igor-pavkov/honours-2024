static void vcpu_clear(struct vcpu_vmx *vmx)
{
	if (vmx->vcpu.cpu == -1)
		return;
	smp_call_function_single(vmx->vcpu.cpu, __vcpu_clear, vmx, 1);
}