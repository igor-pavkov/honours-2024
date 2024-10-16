INST_HANDLER (muls) {	// MULS Rd, Rr
	int d = (buf[0] >> 4 & 0x0f) + 16;
	int r = (buf[0] & 0x0f) + 16;

	ESIL_A ("r%d,DUP,0x80,&,?{,0xffff00,|,},", r);	// sign extension Rr
	ESIL_A ("r%d,DUP,0x80,&,?{,0xffff00,|,},", d);	// sign extension Rd
	ESIL_A ("*,");					// 0: (Rd*Rr)
	ESIL_A ("DUP,0xff,&,r0,=,");			// r0 = LO(0)
	ESIL_A ("8,0,RPICK,>>,0xff,&,r1,=,");		// r1 = HI(0)
	ESIL_A ("DUP,0x8000,&,!,!,cf,=,");		// C = R/15
	ESIL_A ("DUP,!,zf,=,");				// Z = !R
}