static void print_msr_bits(unsigned long val)
{
	pr_cont("<");
	print_bits(val, msr_bits, ",");
	print_tm_bits(val);
	pr_cont(">");
}