static void kvm_cpu_vmxoff(void)
{
	asm volatile (__ex(ASM_VMX_VMXOFF) : : : "cc");
	write_cr4(read_cr4() & ~X86_CR4_VMXE);
}