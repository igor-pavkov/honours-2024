noinline void vmwrite_error(unsigned long field, unsigned long value)
{
	vmx_insn_failed("kvm: vmwrite failed: field=%lx val=%lx err=%d\n",
			field, value, vmcs_read32(VM_INSTRUCTION_ERROR));
}