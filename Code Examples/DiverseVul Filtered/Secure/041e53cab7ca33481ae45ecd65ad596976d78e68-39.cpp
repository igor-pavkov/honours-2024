INST_HANDLER (mov) {	// MOV Rd, Rr
	int d = ((buf[1] << 4) & 0x10) | ((buf[0] >> 4) & 0x0f);
	int r = ((buf[1] << 3) & 0x10) | (buf[0] & 0x0f);
	ESIL_A ("r%d,r%d,=,", r, d);
}