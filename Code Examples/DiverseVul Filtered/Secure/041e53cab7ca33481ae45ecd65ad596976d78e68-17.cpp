INST_HANDLER (elpm) {	// ELPM
			// ELPM Rd
			// ELPM Rd, Z+
	int d = ((buf[1] & 0xfe) == 0x90)
			? ((buf[1] & 1) << 4) | ((buf[0] >> 4) & 0xf)	// Rd
			: 0;						// R0
	ESIL_A ("16,rampz,<<,z,+,_prog,+,[1],");	// read RAMPZ:Z
	ESIL_A ("r%d,=,", d);				// Rd = [1]
	if ((buf[1] & 0xfe) == 0x90 && (buf[0] & 0xf) == 0x7) {
		ESIL_A ("16,1,z,+,DUP,z,=,>>,1,&,rampz,+=,");	// ++(rampz:z)
	}
}