static void vmcs_write32(unsigned long field, u32 value)
{
	vmcs_writel(field, value);
}