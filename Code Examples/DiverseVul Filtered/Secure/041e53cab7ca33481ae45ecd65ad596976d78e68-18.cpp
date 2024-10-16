INST_HANDLER (bclr) {	// BCLR s
			// CLC
			// CLH
			// CLI
			// CLN
			// CLR
			// CLS
			// CLT
			// CLV
			// CLZ
	int s = (buf[0] >> 4) & 0x7;
	ESIL_A ("0xff,%d,1,<<,^,sreg,&=,", s);
}