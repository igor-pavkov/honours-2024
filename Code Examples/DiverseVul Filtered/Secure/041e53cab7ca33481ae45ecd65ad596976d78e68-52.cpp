INST_HANDLER (eijmp) {	// EIJMP
	ut64 z, eind;
	// read z and eind for calculating jump address on runtime
	r_anal_esil_reg_read (anal->esil, "z",    &z,    NULL);
	r_anal_esil_reg_read (anal->esil, "eind", &eind, NULL);
	// real target address may change during execution, so this value will
	// be changing all the time
	op->jump = ((eind << 16) + z) << 1;
	// jump
	ESIL_A ("1,z,16,eind,<<,+,<<,pc,=,");
	// cycles
	op->cycles = 2;
}