INST_HANDLER (eicall) {	// EICALL
	// push pc in stack
	ESIL_A ("pc,");				// esil is already pointing to
						// next instruction (@ret)
	__generic_push (op, CPU_PC_SIZE (cpu));	// push @ret in stack
	// do a standard EIJMP
	INST_CALL (eijmp);
	// fix cycles
	op->cycles = !STR_BEGINS (cpu->model, "ATxmega") ? 3 : 4;
}