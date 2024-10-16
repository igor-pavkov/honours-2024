INST_HANDLER (bst) {	// BST Rd, b
	if (len < 2) {
		return;
	}
	ESIL_A ("r%d,%d,1,<<,&,!,!,tf,=,",			// tf = Rd/b
		((buf[1] & 1) << 4) | ((buf[0] >> 4) & 0xf),	// r
		buf[0] & 0x7);					// b
}