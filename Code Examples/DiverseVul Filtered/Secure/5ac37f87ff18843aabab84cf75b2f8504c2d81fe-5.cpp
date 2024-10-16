static inline void set_task_gate(unsigned int n, unsigned int gdt_entry)
{
	BUG_ON((unsigned)n > 0xFF);
	_set_gate(n, GATE_TASK, (void *)0, 0, 0, (gdt_entry<<3));
}