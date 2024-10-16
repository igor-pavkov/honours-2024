INST_HANDLER (sbiw) {	// SBIW Rd+1:Rd, K
	int d = ((buf[0] & 0x30) >> 3) + 24;
	int k = (buf[0] & 0xf) | ((buf[0] >> 2) & 0x30);
	op->val = k;
	ESIL_A ("%d,r%d:r%d,-,", k, d + 1, d);		// 0(Rd+1:Rd - Rr)
	ESIL_A ("r%d,0x80,&,!,!,"			// V
		"0,RPICK,0x8000,&,!,"
		"&,vf,=,", d + 1);
	ESIL_A ("0,RPICK,0x8000,&,!,!,nf,=,");		// N
	ESIL_A ("0,RPICK,!,zf,=,");			// Z
	ESIL_A ("r%d,0x80,&,!,"				// C
		"0,RPICK,0x8000,&,!,!,"
		"&,cf,=,", d + 1);
	ESIL_A ("vf,nf,^,sf,=,");			// S
	ESIL_A ("r%d:r%d,=,", d + 1, d);		// Rd = result
}