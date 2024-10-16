R_API int r_anal_var_get_argnum(RAnalVar *var) {
	r_return_val_if_fail (var, -1);
	RAnal *anal = var->fcn->anal;
	if (!var->isarg || var->kind != R_ANAL_VAR_KIND_REG) { // TODO: support bp and sp too
		return -1;
	}
	if (!var->regname) {
		return -1;
	}
	RRegItem *reg = r_reg_get (anal->reg, var->regname, -1);
	if (!reg) {
		return -1;
	}
	int i;
	int arg_max = var->fcn->cc ? r_anal_cc_max_arg (anal, var->fcn->cc) : 0;
	for (i = 0; i < arg_max; i++) {
		const char *reg_arg = r_anal_cc_arg (anal, var->fcn->cc, i);
		if (reg_arg && !strcmp (reg->name, reg_arg)) {
			return i;
		}
	}
	return -1;
}