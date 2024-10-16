INST_HANDLER (andi) {	// ANDI Rd, K
			// CBR Rd, K (= ANDI Rd, 1-K)
	if (len < 2) {
		return;
	}
	int d = ((buf[0] >> 4) & 0xf) + 16;
	int k = ((buf[1] & 0x0f) << 4) | (buf[0] & 0x0f);
	op->val = k;
	ESIL_A ("%d,r%d,&,", k, d);				// 0: Rd & Rr
	__generic_bitop_flags (op);				// up flags
	ESIL_A ("r%d,=,", d);					// Rd = Result
}