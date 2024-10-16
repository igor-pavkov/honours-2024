INST_HANDLER (asr) {	// ASR Rd
	if (len < 2) {
		return;
	}
	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 1) << 4);
	ESIL_A ("1,r%d,>>,r%d,0x80,&,|,", d, d);		// 0: R=(Rd >> 1) | Rd7
	ESIL_A ("r%d,0x1,&,!,!,cf,=,", d);			// C = Rd0
	ESIL_A ("0,RPICK,!,zf,=,");				// Z
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");			// N
	ESIL_A ("nf,cf,^,vf,=,");				// V
	ESIL_A ("nf,vf,^,sf,=,");				// S
	ESIL_A ("r%d,=,", d);					// Rd = R
}