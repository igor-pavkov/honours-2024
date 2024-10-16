INST_HANDLER (call) {	// CALL k
	if (len < 4) {
		return;
	}
	op->jump = (buf[2] << 1)
		 | (buf[3] << 9)
		 | (buf[1] & 0x01) << 23
		 | (buf[0] & 0x01) << 17
		 | (buf[0] & 0xf0) << 14;
	op->fail = op->addr + op->size;
	op->cycles = cpu->pc <= 16 ? 3 : 4;
	if (!STR_BEGINS (cpu->model, "ATxmega")) {
		op->cycles--;	// AT*mega optimizes one cycle
	}
	ESIL_A ("pc,");				// esil is already pointing to
						// next instruction (@ret)
	__generic_push (op, CPU_PC_SIZE (cpu));	// push @ret in stack
	ESIL_A ("%"PFMT64d",pc,=,", op->jump);	// jump!
}