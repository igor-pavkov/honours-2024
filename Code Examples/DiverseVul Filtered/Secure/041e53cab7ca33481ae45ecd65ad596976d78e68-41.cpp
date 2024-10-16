INST_HANDLER (bset) {	// BSET s
			// SEC
			// SEH
			// SEI
			// SEN
			// SER
			// SES
			// SET
			// SEV
			// SEZ
	int s = (buf[0] >> 4) & 0x7;
	ESIL_A ("%d,1,<<,sreg,|=,", s);
}