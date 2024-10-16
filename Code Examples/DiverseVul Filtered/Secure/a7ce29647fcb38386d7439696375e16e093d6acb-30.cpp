R_API RList *r_anal_function_get_var_fields(RAnalFunction *fcn, int kind) {
	if (!fcn) {
		return NULL;
	}
	RList *list = r_list_newf ((RListFree)var_field_free);
	if (kind < 1) {
		kind = R_ANAL_VAR_KIND_BPV; // by default show vars
	}
	void **it;
	r_pvector_foreach (&fcn->vars, it) {
		RAnalVar *var = *it;
		if (var->kind != kind) {
			continue;
		}
		if (var_add_structure_fields_to_list (fcn->anal, var, list)) {
			// this var is a struct and var_add_structure_fields_to_list added all the fields
			continue;
		}
		RAnalVarField *field = R_NEW0 (RAnalVarField);
		if (!field) {
			break;
		}
		field->name = strdup (var->name);
		if (!field->name) {
			var_field_free (field);
			break;
		}
		field->delta = var->delta;
		r_list_push (list, field);
	}
	return list;
}