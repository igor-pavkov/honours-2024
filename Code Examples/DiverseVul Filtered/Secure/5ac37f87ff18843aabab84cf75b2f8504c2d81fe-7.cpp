static inline void pack_gate(gate_desc *gate, unsigned char type,
			     unsigned long base, unsigned dpl, unsigned flags,
			     unsigned short seg)
{
	gate->a = (seg << 16) | (base & 0xffff);
	gate->b = (base & 0xffff0000) |
		  (((0x80 | type | (dpl << 5)) & 0xff) << 8);
}