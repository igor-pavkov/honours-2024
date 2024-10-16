R_API ut64 r_anal_var_addr(RAnalVar *var) {
	r_return_val_if_fail (var, UT64_MAX);
	RAnal *anal = var->fcn->anal;
	const char *regname = NULL;
	if (var->kind == R_ANAL_VAR_KIND_BPV) {
		regname = r_reg_get_name (anal->reg, R_REG_NAME_BP);
		return r_reg_getv (anal->reg, regname) + var->delta + var->fcn->bp_off;
	} else if (var->kind == R_ANAL_VAR_KIND_SPV) {
		regname = r_reg_get_name (anal->reg, R_REG_NAME_SP);
		return r_reg_getv (anal->reg, regname) + var->delta;
	}
	return 0;
}