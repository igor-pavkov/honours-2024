INST_HANDLER (mul) {	// MUL Rd, Rr
	int d = ((buf[1] << 4) & 0x10) | ((buf[0] >> 4) & 0x0f);
	int r = ((buf[1] << 3) & 0x10) | (buf[0] & 0x0f);

	ESIL_A ("r%d,r%d,*,", r, d);			// 0: (Rd*Rr)<<1
	ESIL_A ("DUP,0xff,&,r0,=,");			// r0 = LO(0)
	ESIL_A ("8,0,RPICK,>>,0xff,&,r1,=,");		// r1 = HI(0)
	ESIL_A ("DUP,0x8000,&,!,!,cf,=,");		// C = R/15
	ESIL_A ("DUP,!,zf,=,");				// Z = !R
}