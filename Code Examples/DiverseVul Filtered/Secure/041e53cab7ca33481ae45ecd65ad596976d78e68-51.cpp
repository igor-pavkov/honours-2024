INST_HANDLER (std) {	// ST Y, Rr	ST Z, Rr
			// ST Y+, Rr	ST Z+, Rr
			// ST -Y, Rr	ST -Z, Rr
			// ST Y+q, Rr	ST Z+q, Rr
	// load register
	ESIL_A ("r%d,", ((buf[1] & 1) << 4) | ((buf[0] >> 4) & 0xf));
	// write in memory
	__generic_ld_st (
		op, "ram",
		buf[0] & 0x8 ? 'y' : 'z',	// index register Y/Z
		0,				// no use RAMP* registers
		!(buf[1] & 0x10)
			? 0			// no increment
			: buf[0] & 0x1
				? 1		// post incremented
				: -1,		// pre decremented
		!(buf[1] & 0x10)
			? (buf[1] & 0x20)	// offset
			| ((buf[1] & 0xc) << 1)
			| (buf[0] & 0x7)
			: 0,			// no offset
		1);				// load operation (!st)
//	// cycles
//	op->cycles =
//		buf[1] & 0x1 == 0
//			? !(offset ? 1 : 3)		// LDD
//			: buf[0] & 0x3 == 0
//				? 1			// LD Rd, X
//				: buf[0] & 0x3 == 1
//					? 2		// LD Rd, X+
//					: 3;		// LD Rd, -X
//	if (!STR_BEGINS (cpu->model, "ATxmega") && op->cycles > 1) {
//		// AT*mega optimizes 1 cycle!
//		op->cycles--;
//	}
}