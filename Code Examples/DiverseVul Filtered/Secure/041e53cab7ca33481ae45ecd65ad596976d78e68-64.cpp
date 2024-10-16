INST_HANDLER (neg) {	// NEG Rd
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	ESIL_A ("r%d,0x00,-,0xff,&,", d);			// 0: (0-Rd)
	ESIL_A ("DUP,r%d,0xff,^,|,0x08,&,!,!,hf,=,", d);	// H
	ESIL_A ("DUP,0x80,-,!,vf,=,", d);			// V
	ESIL_A ("DUP,0x80,&,!,!,nf,=,");			// N
	ESIL_A ("DUP,!,zf,=,");					// Z
	ESIL_A ("DUP,!,!,cf,=,");				// C
	ESIL_A ("vf,nf,^,sf,=,");				// S
	ESIL_A ("r%d,=,", d);					// Rd = result
}