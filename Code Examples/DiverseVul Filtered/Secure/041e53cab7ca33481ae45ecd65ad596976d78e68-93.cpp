INST_HANDLER (brbx) {	// BRBC s, k
			// BRBS s, k
			// BRBC/S 0:		BRCC		BRCS
			//			BRSH		BRLO
			// BRBC/S 1:		BREQ		BRNE
			// BRBC/S 2:		BRPL		BRMI
			// BRBC/S 3:		BRVC		BRVS
			// BRBC/S 4:		BRGE		BRLT
			// BRBC/S 5:		BRHC		BRHS
			// BRBC/S 6:		BRTC		BRTS
			// BRBC/S 7:		BRID		BRIE
	int s = buf[0] & 0x7;
	op->jump = op->addr
		+ ((((buf[1] & 0x03) << 6) | ((buf[0] & 0xf8) >> 2))
			| (buf[1] & 0x2 ? ~((int) 0x7f) : 0))
		+ 2;
	op->fail = op->addr + op->size;
	op->cycles = 1;	// XXX: This is a bug, because depends on eval state,
			// so it cannot be really be known until this
			// instruction is executed by the ESIL interpreter!!!
			// In case of evaluating to true, this instruction
			// needs 2 cycles, elsewhere it needs only 1 cycle.
	ESIL_A ("%d,1,<<,sreg,&,", s);				// SREG(s)
	ESIL_A (buf[1] & 0x4
			? "!,"		// BRBC => branch if cleared
			: "!,!,");	// BRBS => branch if set
	ESIL_A ("?{,%"PFMT64d",pc,=,},", op->jump);	// ?true => jmp
}