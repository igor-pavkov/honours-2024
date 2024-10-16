static void vmcs_write16(unsigned long field, u16 value)
{
	vmcs_writel(field, value);
}