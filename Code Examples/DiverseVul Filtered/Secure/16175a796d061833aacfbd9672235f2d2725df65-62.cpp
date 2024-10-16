static int handle_nmi_window(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	u32 cpu_based_vm_exec_control;

	/* clear pending NMI */
	cpu_based_vm_exec_control = vmcs_read32(CPU_BASED_VM_EXEC_CONTROL);
	cpu_based_vm_exec_control &= ~CPU_BASED_VIRTUAL_NMI_PENDING;
	vmcs_write32(CPU_BASED_VM_EXEC_CONTROL, cpu_based_vm_exec_control);
	++vcpu->stat.nmi_window_exits;

	return 1;
}