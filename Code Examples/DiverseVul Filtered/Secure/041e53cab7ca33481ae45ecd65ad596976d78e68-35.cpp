INST_HANDLER (lsr) {	// LSR Rd
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	ESIL_A ("1,r%d,>>,", d);				// 0: R=(Rd >> 1)
	ESIL_A ("r%d,0x1,&,!,!,cf,=,", d);			// C = Rd0
	ESIL_A ("0,RPICK,!,zf,=,");				// Z
	ESIL_A ("0,nf,=,");					// N
	ESIL_A ("nf,cf,^,vf,=,");				// V
	ESIL_A ("nf,vf,^,sf,=,");				// S
	ESIL_A ("r%d,=,", d);					// Rd = R
}