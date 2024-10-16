R_API bool r_anal_function_rebase_vars(RAnal *a, RAnalFunction *fcn) {
	r_return_val_if_fail (a && fcn, false);
	RListIter *it;
	RAnalVar *var;
	RList *var_list = r_anal_var_all_list (a, fcn);
	r_return_val_if_fail (var_list, false);

	r_list_foreach (var_list, it, var) {
		// Resync delta in case the registers list changed
		if (var->isarg && var->kind == 'r') {
			RRegItem *reg = r_reg_get (a->reg, var->regname, -1);
			if (reg) {
				if (var->delta != reg->index) {
					var->delta = reg->index;
				}
			}
		}
	}

	r_list_free (var_list);
	return true;
}