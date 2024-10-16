static inline void set_system_gate(unsigned int n, void *addr)
{
	BUG_ON((unsigned)n > 0xFF);
#ifdef CONFIG_X86_32
	_set_gate(n, GATE_TRAP, addr, 0x3, 0, __KERNEL_CS);
#else
	_set_gate(n, GATE_INTERRUPT, addr, 0x3, 0, __KERNEL_CS);
#endif
}