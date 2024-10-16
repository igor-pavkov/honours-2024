INST_HANDLER (com) {	// COM Rd
	int r = ((buf[0] >> 4) & 0x0f) | ((buf[1] & 1) << 4);

	ESIL_A ("r%d,0xff,-,0xff,&,r%d,=,", r, r);		// Rd = 0xFF-Rd
								// FLAGS:
	ESIL_A ("0,cf,=,");					// C
	__generic_bitop_flags (op);				// ...rest...
}