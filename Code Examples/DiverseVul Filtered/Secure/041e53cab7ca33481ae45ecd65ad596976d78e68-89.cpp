INST_HANDLER (ret) {	// RET
	op->eob = true;
	// esil
	__generic_pop (op, CPU_PC_SIZE (cpu));
	ESIL_A ("pc,=,");	// jump!
	// cycles
	if (CPU_PC_SIZE (cpu) > 2) {	// if we have a bus bigger than 16 bit
		op->cycles++;	// (i.e. a 22-bit bus), add one extra cycle
	}
}