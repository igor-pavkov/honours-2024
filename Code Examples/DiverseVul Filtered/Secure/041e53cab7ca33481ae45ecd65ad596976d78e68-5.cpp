INST_HANDLER (sub) {	// SUB Rd, Rr
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	int r = (buf[0] & 0xf) | ((buf[1] & 2) << 3);

	ESIL_A ("r%d,r%d,-,", r, d);			// 0: (Rd-k)
	__generic_sub_update_flags_rr (op, d, r, 0);	// FLAGS (no carry)
	ESIL_A ("r%d,=,", d);				// Rd = Result
}