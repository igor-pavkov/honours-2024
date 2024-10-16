INST_HANDLER (lds16) {	// LDS Rd, k
	int d = ((buf[0] >> 4) & 0xf) + 16;
	int k = (buf[0] & 0x0f)
		| ((buf[1] << 3) & 0x30)
		| ((buf[1] << 4) & 0x40)
		| (~(buf[1] << 4) & 0x80);
	op->ptr = k;

	// load value from @k
	__generic_ld_st (op, "ram", 0, 0, 0, k, 0);
	ESIL_A ("r%d,=,", d);
}