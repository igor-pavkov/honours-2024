INST_HANDLER (out) {	// OUT A, Rr
	int r = ((buf[0] >> 4) & 0x0f) | ((buf[1] & 0x01) << 4);
	int a = (buf[0] & 0x0f) | ((buf[1] & 0x6) << 3);
	RStrBuf *io_dst = __generic_io_dest (a, 1, cpu);
	op->type2 = 1;
	op->val = a;
	op->family = R_ANAL_OP_FAMILY_IO;
	ESIL_A ("r%d,%s,", r, r_strbuf_get (io_dst));
	r_strbuf_free (io_dst);
}