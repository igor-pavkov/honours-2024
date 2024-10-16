static int avr_op(RAnal *anal, RAnalOp *op, ut64 addr, const ut8 *buf, int len) {
	CPU_MODEL *cpu;
	ut64 offset;

	// init op
	if (!op) {
		return 2;
	}

	// select cpu info
	cpu = get_cpu_model (anal->cpu);

	// set memory layout registers
	if (anal->esil) {
		offset = 0;
		r_anal_esil_reg_write (anal->esil, "_prog", offset);

		offset += (1 << cpu->pc);
		r_anal_esil_reg_write (anal->esil, "_io", offset);

		offset += const_get_value (const_by_name (cpu, CPU_CONST_PARAM, "sram_start"));
		r_anal_esil_reg_write (anal->esil, "_sram", offset);

		offset += const_get_value (const_by_name (cpu, CPU_CONST_PARAM, "sram_size"));
		r_anal_esil_reg_write (anal->esil, "_eeprom", offset);

		offset += const_get_value (const_by_name (cpu, CPU_CONST_PARAM, "eeprom_size"));
		r_anal_esil_reg_write (anal->esil, "_page", offset);
	}
	// process opcode
	avr_op_analyze (anal, op, addr, buf, len, cpu);

	return op->size;
}