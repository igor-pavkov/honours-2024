R_API RAnalVar *r_anal_function_set_var(RAnalFunction *fcn, int delta, char kind, R_NULLABLE const char *type, int size, bool isarg, R_NONNULL const char *name) {
	r_return_val_if_fail (fcn && name, NULL);
	RAnalVar *existing = r_anal_function_get_var_byname (fcn, name);
	if (existing && (existing->kind != kind || existing->delta != delta)) {
		// var name already exists at a different kind+delta
		return NULL;
	}
	RRegItem *reg = NULL;
	if (!kind) {
		kind = R_ANAL_VAR_KIND_BPV;
	}
	if (!type) {
		type = __int_type_from_size (size);
		if (!type) {
			type = __int_type_from_size (fcn->anal->bits);
		}
		if (!type) {
			type = "int32_t";
		}
	}
	if (!valid_var_kind (kind)) {
		eprintf ("Invalid var kind '%c'\n", kind);
		return NULL;
	}
	if (kind == R_ANAL_VAR_KIND_REG) {
		reg = r_reg_index_get (fcn->anal->reg, R_ABS (delta));
		if (!reg) {
			if (fcn->anal->verbose) {
				eprintf ("No register at index %d\n", delta);
			}
			return NULL;
		}
	}
	RAnalVar *var = r_anal_function_get_var (fcn, kind, delta);
	if (!var) {
		var = R_NEW0 (RAnalVar);
		if (!var) {
			return NULL;
		}
		r_pvector_push (&fcn->vars, var);
		var->fcn = fcn;
		r_vector_init (&var->accesses, sizeof (RAnalVarAccess), NULL, NULL);
		r_vector_init (&var->constraints, sizeof (RAnalVarConstraint), NULL, NULL);
	} else {
		free (var->name);
		free (var->regname);
		free (var->type);
	}
	R_DIRTY (fcn->anal);
	var->name = strdup (name);
	var->regname = reg? strdup (reg->name): NULL; // TODO: no strdup here? pool? or not keep regname at all?
	var->type = strdup (type);
	var->kind = kind;
	var->isarg = isarg;
	var->delta = delta;
	shadow_var_struct_members (var);
	return var;
}