INST_HANDLER (sbix) {	// SBIC A, b
			// SBIS A, b
	int a = (buf[0] >> 3) & 0x1f;
	int b = buf[0] & 0x07;
	RAnalOp next_op;
	RStrBuf *io_port;

	op->type2 = 0;
	op->val = a;
	op->family = R_ANAL_OP_FAMILY_IO;

	// calculate next instruction size (call recursively avr_op_analyze)
	// and free next_op's esil string (we dont need it now)
	avr_op_analyze (anal,
			&next_op,
			op->addr + op->size, buf + op->size,
			len - op->size,
			cpu);
	r_strbuf_fini (&next_op.esil);
	op->jump = op->addr + next_op.size + 2;

	// cycles
	op->cycles = 1;	// XXX: This is a bug, because depends on eval state,
			// so it cannot be really be known until this
			// instruction is executed by the ESIL interpreter!!!
			// In case of evaluating to false, this instruction
			// needs 2/3 cycles, elsewhere it needs only 1 cycle.

	// read port a and clear bit b
	io_port = __generic_io_dest (a, 0, cpu);
	ESIL_A ("%d,1,<<,%s,&,", b, io_port);		// IO(A,b)
	ESIL_A ((buf[1] & 0xe) == 0xc
			? "!,"				// SBIC => branch if 0
			: "!,!,");			// SBIS => branch if 1
	ESIL_A ("?{,%"PFMT64d",pc,=,},", op->jump);	// ?true => jmp
	r_strbuf_free (io_port);
}