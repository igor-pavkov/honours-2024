INST_HANDLER (or) {	// OR Rd, Rr
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	int r = (buf[0] & 0xf) | ((buf[1] & 2) << 3);
	ESIL_A ("r%d,r%d,|,", r, d);				// 0: (Rd | Rr)
	ESIL_A ("0,RPICK,!,zf,=,");				// Z
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");			// N
	ESIL_A ("0,vf,=,");					// V
	ESIL_A ("nf,sf,=,");					// S
	ESIL_A ("r%d,=,", d);					// Rd = result
}