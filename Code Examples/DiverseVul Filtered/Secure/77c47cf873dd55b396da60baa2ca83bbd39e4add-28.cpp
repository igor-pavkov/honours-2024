static RAnalValue *anal_fill_r0_reg_ref(RAnal *anal, int reg, st64 size){
	RAnalValue *ret = anal_fill_ai_rg (anal, 0);
	ret->regdelta = r_reg_get (anal->reg, regs[reg], R_REG_TYPE_GPR);
	ret->memref = size;
	return ret;
}