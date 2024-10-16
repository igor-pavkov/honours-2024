static void move_msr_up(struct vcpu_vmx *vmx, int from, int to)
{
	struct kvm_msr_entry tmp;

	tmp = vmx->guest_msrs[to];
	vmx->guest_msrs[to] = vmx->guest_msrs[from];
	vmx->guest_msrs[from] = tmp;
	tmp = vmx->host_msrs[to];
	vmx->host_msrs[to] = vmx->host_msrs[from];
	vmx->host_msrs[from] = tmp;
}