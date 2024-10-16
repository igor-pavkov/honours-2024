static RAnalValue *anal_regrel_jump(RAnal* anal, RAnalOp* op, ut8 reg){
	RAnalValue *ret = r_anal_value_new ();
	ret->reg = r_reg_get (anal->reg, regs[reg], R_REG_TYPE_GPR);
	ret->base = op->addr+4;
	return ret;
}