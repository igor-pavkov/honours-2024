INST_HANDLER (ori) {	// ORI Rd, K
			// SBR Rd, K
	int d = ((buf[0] >> 4) & 0xf) + 16;
	int k = (buf[0] & 0xf) | ((buf[1] & 0xf) << 4);
	op->val = k;
	ESIL_A ("r%d,%d,|,", d, k);				// 0: (Rd | k)
	ESIL_A ("0,RPICK,!,zf,=,");				// Z
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");			// N
	ESIL_A ("0,vf,=,");					// V
	ESIL_A ("nf,sf,=,");					// S
	ESIL_A ("r%d,=,", d);					// Rd = result
}