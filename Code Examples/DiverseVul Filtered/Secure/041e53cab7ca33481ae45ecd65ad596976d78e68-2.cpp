INST_HANDLER (add) {	// ADD Rd, Rr
			// LSL Rd
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	int r = (buf[0] & 0xf) | ((buf[1] & 2) << 3);
	ESIL_A ("r%d,r%d,+,", r, d);			// Rd + Rr
	__generic_add_update_flags_rr(op, d, r);	// FLAGS
	ESIL_A ("r%d,=,", d);				// Rd = result
}