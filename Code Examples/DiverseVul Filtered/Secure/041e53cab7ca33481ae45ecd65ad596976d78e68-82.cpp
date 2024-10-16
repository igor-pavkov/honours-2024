INST_HANDLER (rjmp) {	// RJMP k
	op->jump = (op->addr
#ifdef _MSC_VER
#pragma message ("anal_avr.c: WARNING: Probably broken on windows")
		+ ((((( buf[1] & 0xf) << 9) | (buf[0] << 1)))
			| (buf[1] & 0x8 ? ~(0x1fff) : 0))
#else
		+ ((((( (typeof (op->jump)) buf[1] & 0xf) << 9) | ((typeof (op->jump)) buf[0] << 1)))
			| (buf[1] & 0x8 ? ~((typeof (op->jump)) 0x1fff) : 0))
#endif
		+ 2) & CPU_PC_MASK (cpu);
	ESIL_A ("%"PFMT64d",pc,=,", op->jump);
}