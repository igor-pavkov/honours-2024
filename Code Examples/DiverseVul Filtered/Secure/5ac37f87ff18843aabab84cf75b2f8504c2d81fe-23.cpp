static inline void pack_gate(gate_desc *gate, unsigned type, unsigned long func,
			     unsigned dpl, unsigned ist, unsigned seg)
{
	gate->offset_low = PTR_LOW(func);
	gate->segment = __KERNEL_CS;
	gate->ist = ist;
	gate->p = 1;
	gate->dpl = dpl;
	gate->zero0 = 0;
	gate->zero1 = 0;
	gate->type = type;
	gate->offset_middle = PTR_MIDDLE(func);
	gate->offset_high = PTR_HIGH(func);
}