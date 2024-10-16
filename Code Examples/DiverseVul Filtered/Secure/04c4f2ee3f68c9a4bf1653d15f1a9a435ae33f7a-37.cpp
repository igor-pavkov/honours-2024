u32 vmx_pin_based_exec_ctrl(struct vcpu_vmx *vmx)
{
	u32 pin_based_exec_ctrl = vmcs_config.pin_based_exec_ctrl;

	if (!kvm_vcpu_apicv_active(&vmx->vcpu))
		pin_based_exec_ctrl &= ~PIN_BASED_POSTED_INTR;

	if (!enable_vnmi)
		pin_based_exec_ctrl &= ~PIN_BASED_VIRTUAL_NMIS;

	if (!enable_preemption_timer)
		pin_based_exec_ctrl &= ~PIN_BASED_VMX_PREEMPTION_TIMER;

	return pin_based_exec_ctrl;
}