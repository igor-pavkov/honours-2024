INST_HANDLER (and) {	// AND Rd, Rr
			// TST Rd
	if (len < 2) {
		return;
	}
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	int r = (buf[0] & 0xf) | ((buf[1] & 2) << 3);
	ESIL_A ("r%d,r%d,&,", r, d);				// 0: Rd & Rr
	__generic_bitop_flags (op);				// up flags
	ESIL_A ("r%d,=,", d);					// Rd = Result
}