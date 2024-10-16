INST_HANDLER (icall) {	// ICALL k
	// push pc in stack
	ESIL_A ("pc,");				// esil is already pointing to
						// next instruction (@ret)
	__generic_push (op, CPU_PC_SIZE (cpu));	// push @ret in stack
	// do a standard IJMP
	INST_CALL (ijmp);
	// fix cycles
	if (!STR_BEGINS (cpu->model, "ATxmega")) {
		// AT*mega optimizes 1 cycle!
		op->cycles--;
	}
}