static void print_tm_bits(unsigned long val)
{
/*
 * This only prints something if at least one of the TM bit is set.
 * Inside the TM[], the output means:
 *   E: Enabled		(bit 32)
 *   S: Suspended	(bit 33)
 *   T: Transactional	(bit 34)
 */
	if (val & (MSR_TM | MSR_TS_S | MSR_TS_T)) {
		pr_cont(",TM[");
		print_bits(val, msr_tm_bits, "");
		pr_cont("]");
	}
}