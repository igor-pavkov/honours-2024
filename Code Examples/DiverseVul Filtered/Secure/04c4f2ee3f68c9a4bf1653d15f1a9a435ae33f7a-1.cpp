static u64 vmx_read_guest_kernel_gs_base(struct vcpu_vmx *vmx)
{
	preempt_disable();
	if (vmx->guest_state_loaded)
		rdmsrl(MSR_KERNEL_GS_BASE, vmx->msr_guest_kernel_gs_base);
	preempt_enable();
	return vmx->msr_guest_kernel_gs_base;
}