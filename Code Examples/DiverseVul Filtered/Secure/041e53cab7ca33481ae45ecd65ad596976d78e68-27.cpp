INST_HANDLER (sbci) {	// SBCI Rd, k
	int d = ((buf[0] >> 4) & 0xf) + 16;
	int k = ((buf[1] & 0xf) << 4) | (buf[0] & 0xf);
	op->val = k;

	ESIL_A ("cf,%d,+,r%d,-,", k, d);		// 0: (Rd-k-C)
	__generic_sub_update_flags_rk (op, d, k, 1);	// FLAGS (carry)
	ESIL_A ("r%d,=,", d);				// Rd = Result
}