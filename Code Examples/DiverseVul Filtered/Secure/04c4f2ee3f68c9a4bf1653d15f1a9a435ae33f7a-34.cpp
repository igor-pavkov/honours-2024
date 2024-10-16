noinline void vmptrld_error(struct vmcs *vmcs, u64 phys_addr)
{
	vmx_insn_failed("kvm: vmptrld failed: %p/%llx\n", vmcs, phys_addr);
}