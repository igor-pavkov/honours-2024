static RAnalValue *anal_fill_reg_disp_mem(RAnal *anal, int reg, st64 delta, st64 size) {
	RAnalValue *ret = anal_fill_ai_rg (anal, reg);
	ret->memref = size;
	ret->delta = delta*size;
	return ret;
}