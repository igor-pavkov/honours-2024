static unsigned long vmcs_readl(unsigned long field)
{
	unsigned long value;

	asm volatile (__ex(ASM_VMX_VMREAD_RDX_RAX)
		      : "=a"(value) : "d"(field) : "cc");
	return value;
}