INST_HANDLER (bld) {	// BLD Rd, b
	if (len < 2) {
		return;
	}
	int d = ((buf[1] & 0x01) << 4) | ((buf[0] >> 4) & 0xf);
	int b = buf[0] & 0x7;
	ESIL_A ("r%d,%d,1,<<,0xff,^,&,", d, b);			// Rd/b = 0
	ESIL_A ("%d,tf,<<,|,r%d,=,", b, d);			// Rd/b |= T<<b
}