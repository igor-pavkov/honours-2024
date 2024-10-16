static void exit_lmode(struct kvm_vcpu *vcpu)
{
	vcpu->arch.shadow_efer &= ~EFER_LMA;

	vmcs_write32(VM_ENTRY_CONTROLS,
		     vmcs_read32(VM_ENTRY_CONTROLS)
		     & ~VM_ENTRY_IA32E_MODE);
}