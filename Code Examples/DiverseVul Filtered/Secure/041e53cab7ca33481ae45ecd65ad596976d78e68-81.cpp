INST_HANDLER (ror) {	// ROR Rd
	int d = ((buf[0] >> 4) & 0x0f) | ((buf[1] << 4) & 0x10);
	ESIL_A ("1,r%d,>>,7,cf,<<,|,", d);		// 0: (Rd>>1) | (cf<<7)
	ESIL_A ("r%d,1,&,cf,=,", d);			// C
	ESIL_A ("0,RPICK,!,zf,=,");			// Z
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");		// N
	ESIL_A ("nf,cf,^,vf,=,");			// V
	ESIL_A ("vf,nf,^,sf,=,");			// S
	ESIL_A ("r%d,=,", d);				// Rd = result
}