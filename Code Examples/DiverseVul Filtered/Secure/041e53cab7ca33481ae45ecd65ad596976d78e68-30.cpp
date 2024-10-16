INST_HANDLER (fmul) {	// FMUL Rd, Rr
	int d = ((buf[0] >> 4) & 0x7) + 16;
	int r = (buf[0] & 0x7) + 16;

	ESIL_A ("1,r%d,r%d,*,<<,", r, d);		// 0: (Rd*Rr)<<1
	ESIL_A ("0xffff,&,");				// prevent overflow
	ESIL_A ("DUP,0xff,&,r0,=,");			// r0 = LO(0)
	ESIL_A ("8,0,RPICK,>>,0xff,&,r1,=,");		// r1 = HI(0)
	ESIL_A ("DUP,0x8000,&,!,!,cf,=,");		// C = R/16
	ESIL_A ("DUP,!,zf,=,");				// Z = !R
}