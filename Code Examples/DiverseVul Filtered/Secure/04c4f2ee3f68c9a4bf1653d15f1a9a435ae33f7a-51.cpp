asmlinkage void vmread_error(unsigned long field, bool fault)
{
	if (fault)
		kvm_spurious_fault();
	else
		vmx_insn_failed("kvm: vmread failed: field=%lx\n", field);
}