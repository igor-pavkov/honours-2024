INST_HANDLER (in) {	// IN Rd, A
	int r = ((buf[0] >> 4) & 0x0f) | ((buf[1] & 0x01) << 4);
	int a = (buf[0] & 0x0f) | ((buf[1] & 0x6) << 3);
	RStrBuf *io_src = __generic_io_dest (a, 0, cpu);
	op->type2 = 0;
	op->val = a;
	op->family = R_ANAL_OP_FAMILY_IO;
	ESIL_A ("%s,r%d,=,", r_strbuf_get (io_src), r);
	r_strbuf_free (io_src);
}