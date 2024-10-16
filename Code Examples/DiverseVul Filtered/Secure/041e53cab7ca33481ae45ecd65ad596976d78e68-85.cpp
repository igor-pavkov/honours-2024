INST_HANDLER (ijmp) {	// IJMP k
	ut64 z;
	// read z for calculating jump address on runtime
	r_anal_esil_reg_read (anal->esil, "z", &z, NULL);
	// real target address may change during execution, so this value will
	// be changing all the time
	op->jump = z << 1;
	op->cycles = 2;
	ESIL_A ("1,z,<<,pc,=,");		// jump!
}