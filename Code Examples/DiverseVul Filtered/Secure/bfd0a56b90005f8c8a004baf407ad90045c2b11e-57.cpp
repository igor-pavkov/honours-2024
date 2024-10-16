static void add_atomic_switch_msr_special(unsigned long entry,
		unsigned long exit, unsigned long guest_val_vmcs,
		unsigned long host_val_vmcs, u64 guest_val, u64 host_val)
{
	vmcs_write64(guest_val_vmcs, guest_val);
	vmcs_write64(host_val_vmcs, host_val);
	vmcs_set_bits(VM_ENTRY_CONTROLS, entry);
	vmcs_set_bits(VM_EXIT_CONTROLS, exit);
}