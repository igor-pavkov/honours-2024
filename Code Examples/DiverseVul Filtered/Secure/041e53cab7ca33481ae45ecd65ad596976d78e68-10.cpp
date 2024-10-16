static int avr_custom_spm_page_write(RAnalEsil *esil) {
	CPU_MODEL *cpu;
	char *t = NULL;
	ut64 addr, page_size_bits, tmp_page;

	// sanity check
	if (!esil || !esil->anal || !esil->anal->reg) {
		return false;
	}

	// get target address
	if (!__esil_pop_argument (esil, &addr)) {
		return false;
	}

	// get details about current MCU and fix input address and base address
	// of the internal temporary page
	cpu = get_cpu_model (esil->anal->cpu);
	page_size_bits = const_get_value (const_by_name (cpu, CPU_CONST_PARAM, "page_size"));
	r_anal_esil_reg_read (esil, "_page", &tmp_page, NULL);

	// align base address to page_size_bits
	addr &= (~(MASK (page_size_bits)) & CPU_PC_MASK (cpu));

	// perform writing
	//eprintf ("SPM_PAGE_WRITE %ld bytes @ 0x%08" PFMT64x ".\n", page_size, addr);
	if (!(t = malloc (1 << page_size_bits))) {
		eprintf ("Cannot alloc a buffer for copying the temporary page.\n");
		return false;
	}
	r_anal_esil_mem_read (esil, tmp_page, (ut8 *) t, 1 << page_size_bits);
	r_anal_esil_mem_write (esil, addr, (ut8 *) t, 1 << page_size_bits);

	return true;
}