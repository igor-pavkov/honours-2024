INST_HANDLER (swap) {	// SWAP Rd
	int d = ((buf[1] & 0x1) << 4) | ((buf[0] >> 4) & 0xf);
	ESIL_A ("4,r%d,>>,0x0f,&,", d);		// (Rd >> 4) & 0xf
	ESIL_A ("4,r%d,<<,0xf0,&,", d);		// (Rd >> 4) & 0xf
	ESIL_A ("|,", d);			// S[0] | S[1]
	ESIL_A ("r%d,=,", d);			// Rd = result
}