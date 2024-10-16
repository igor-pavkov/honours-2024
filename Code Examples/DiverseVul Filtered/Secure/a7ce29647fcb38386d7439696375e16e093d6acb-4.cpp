R_API void r_anal_var_set_type(RAnalVar *var, const char *type) {
	char *nt = strdup (type);
	if (!nt) {
		return;
	}
	free (var->type);
	var->type = nt;
	shadow_var_struct_members (var);
}