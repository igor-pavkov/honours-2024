R_API R_OWN char *r_anal_function_autoname_var(RAnalFunction *fcn, char kind, const char *pfx, int ptr) {
	void **it;
	const ut32 uptr = R_ABS (ptr);
	char *varname = r_str_newf ("%s_%xh", pfx, uptr);
	r_pvector_foreach (&fcn->vars, it) {
		RAnalVar *var = *it;
		if (!strcmp (varname, var->name)) {
			if (var->kind != kind) {
				const char *k = kind == R_ANAL_VAR_KIND_SPV ? "sp" : "bp";
				free (varname);
				varname = r_str_newf ("%s_%s_%xh", pfx, k, uptr);
				return varname;
			}
			int i = 2;
			do {
				free (varname);
				varname = r_str_newf ("%s_%xh_%u", pfx, uptr, i++);
			} while (r_anal_function_get_var_byname (fcn, varname));
			return varname;
		}
	}
	return varname;
}