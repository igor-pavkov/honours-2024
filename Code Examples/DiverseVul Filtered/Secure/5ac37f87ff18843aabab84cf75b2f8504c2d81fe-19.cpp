static inline void native_write_idt_entry(gate_desc *idt, int entry,
					  const gate_desc *gate)
{
	memcpy(&idt[entry], gate, sizeof(*gate));
}