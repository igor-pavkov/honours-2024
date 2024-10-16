R_API bool r_anal_var_rename(RAnalVar *var, const char *new_name, bool verbose) {
	r_return_val_if_fail (var, false);
	if (!r_anal_var_check_name (new_name)) {
		return false;
	}
	RAnalVar *v1 = r_anal_function_get_var_byname (var->fcn, new_name);
	if (v1) {
		if (verbose) {
			eprintf ("variable or arg with name `%s` already exist\n", new_name);
		}
		return false;
	}
	char *nn = strdup (new_name);
	if (!nn) {
		return false;
	}
	free (var->name);
	var->name = nn;
	return true;
}