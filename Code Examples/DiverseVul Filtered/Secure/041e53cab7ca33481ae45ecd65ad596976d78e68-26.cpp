INST_HANDLER (lpm) {	// LPM
			// LPM Rd, Z
			// LPM Rd, Z+
	ut16 ins = (((ut16) buf[1]) << 8) | ((ut16) buf[0]);
	// read program memory
	__generic_ld_st (
		op, "prog",
		'z',				// index register Y/Z
		1,				// use RAMP* registers
		(ins & 0xfe0f) == 0x9005
			? 1			// post incremented
			: 0,			// no increment
		0,				// not offset
		0);				// load operation (!st)
	// load register
	ESIL_A ("r%d,=,",
		(ins == 0x95c8)
			? 0			// LPM (r0)
			: ((buf[0] >> 4) & 0xf)	// LPM Rd
				| ((buf[1] & 0x1) << 4));
}