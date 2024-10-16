static inline bool arch_destroys_dst(const char *arch) {
	return (STR_EQUAL (arch, "arm") || STR_EQUAL (arch, "riscv") || STR_EQUAL (arch, "ppc"));
}