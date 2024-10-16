INST_HANDLER (dec) {	// DEC Rd
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 0x1) << 4);
	ESIL_A ("-1,r%d,+,", d);				// --Rd
								// FLAGS:
	ESIL_A ("0,RPICK,0x7f,==,vf,=,");			// V
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");			// N
	ESIL_A ("0,RPICK,!,zf,=,");				// Z
	ESIL_A ("vf,nf,^,sf,=,");				// S
	ESIL_A ("r%d,=,", d);					// Rd = Result
}