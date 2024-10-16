INST_HANDLER (ld) {	// LD Rd, X
			// LD Rd, X+
			// LD Rd, -X
	// read memory
	__generic_ld_st (
		op, "ram",
		'x',				// use index register X
		0,				// no use RAMP* registers
		(buf[0] & 0xf) == 0xe
			? -1			// pre decremented
			: (buf[0] & 0xf) == 0xd
				? 1		// post incremented
				: 0,		// no increment
		0,				// offset always 0
		0);				// load operation (!st)
	// load register
	ESIL_A ("r%d,=,", ((buf[1] & 1) << 4) | ((buf[0] >> 4) & 0xf));
	// cycles
	op->cycles = (buf[0] & 0x3) == 0
			? 2			// LD Rd, X
			: (buf[0] & 0x3) == 1
				? 2		// LD Rd, X+
				: 3;		// LD Rd, -X
	if (!STR_BEGINS (cpu->model, "ATxmega") && op->cycles > 1) {
		// AT*mega optimizes 1 cycle!
		op->cycles--;
	}
}