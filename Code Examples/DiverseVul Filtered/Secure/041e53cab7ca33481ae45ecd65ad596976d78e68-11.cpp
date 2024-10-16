INST_HANDLER (push) {	// PUSH Rr
	int r = ((buf[1] & 0x1) << 4) | ((buf[0] >> 4) & 0xf);
	ESIL_A ("r%d,", r);	// load Rr
	__generic_push (op, 1);	// push it into stack
	// cycles
	op->cycles = !STR_BEGINS (cpu->model, "ATxmega")
			? 1	// AT*mega optimizes one cycle
			: 2;
}