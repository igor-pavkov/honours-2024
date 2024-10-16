static RAnalValue *anal_fill_reg_ref(RAnal *anal, int reg, st64 size){
	RAnalValue *ret = anal_fill_ai_rg (anal, reg);
	ret->memref = size;
	return ret;
}