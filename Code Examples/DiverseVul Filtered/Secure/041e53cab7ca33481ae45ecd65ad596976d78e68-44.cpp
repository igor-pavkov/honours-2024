INST_HANDLER (fmulsu) {	// FMULSU Rd, Rr
	int d = ((buf[0] >> 4) & 0x7) + 16;
	int r = (buf[0] & 0x7) + 16;

	ESIL_A ("1,");
	ESIL_A ("r%d,DUP,0x80,&,?{,0xffff00,|,},", d);	// sign extension Rd
	ESIL_A ("r%d", r);				// unsigned Rr
	ESIL_A ("*,<<,");				// 0: (Rd*Rr)<<1

	ESIL_A ("0xffff,&,");				// prevent overflow
	ESIL_A ("DUP,0xff,&,r0,=,");			// r0 = LO(0)
	ESIL_A ("8,0,RPICK,>>,0xff,&,r1,=,");		// r1 = HI(0)
	ESIL_A ("DUP,0x8000,&,!,!,cf,=,");		// C = R/16
	ESIL_A ("DUP,!,zf,=,");				// Z = !R
}