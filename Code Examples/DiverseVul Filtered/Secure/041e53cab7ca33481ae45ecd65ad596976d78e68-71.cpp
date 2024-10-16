INST_HANDLER (spm) {	// SPM Z+
	ut64 spmcsr;

	// read SPM Control Register (SPMCR)
	r_anal_esil_reg_read (anal->esil, "spmcsr", &spmcsr, NULL);

	// clear SPMCSR
	ESIL_A ("0x7c,spmcsr,&=,");

	// decide action depending on the old value of SPMCSR
	switch (spmcsr & 0x7f) {
		case 0x03: // PAGE ERASE
			// invoke SPM_CLEAR_PAGE (erases target page writing
			// the 0xff value
			ESIL_A ("16,rampz,<<,z,+,"); // push target address
			ESIL_A ("SPM_PAGE_ERASE,");  // do magic
			break;

		case 0x01: // FILL TEMPORARY BUFFER
			ESIL_A ("r1,r0,");           // push data
			ESIL_A ("z,");               // push target address
			ESIL_A ("SPM_PAGE_FILL,");   // do magic
			break;

		case 0x05: // WRITE PAGE
			ESIL_A ("16,rampz,<<,z,+,"); // push target address
			ESIL_A ("SPM_PAGE_WRITE,");  // do magic
			break;

		default:
			eprintf ("SPM: I dont know what to do with SPMCSR %02x.\n",
					(unsigned int) spmcsr);
	}

	op->cycles = 1;	// This is truly false. Datasheets do not publish how
			// many cycles this instruction uses in all its
			// operation modes and I am pretty sure that this value
			// can vary substantially from one MCU type to another.
			// So... one cycle is fine.
}