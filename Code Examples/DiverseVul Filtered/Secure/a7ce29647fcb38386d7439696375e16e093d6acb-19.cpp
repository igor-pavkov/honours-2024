R_API void r_anal_extract_rarg(RAnal *anal, RAnalOp *op, RAnalFunction *fcn, int *reg_set, int *count) {
	int i, argc = 0;
	r_return_if_fail (anal && op && fcn);
	const char *opsreg = op->src[0] ? get_regname (anal, op->src[0]) : NULL;
	const char *opdreg = op->dst ? get_regname (anal, op->dst) : NULL;
	const int size = (fcn->bits ? fcn->bits : anal->bits) / 8;
	if (!fcn->cc) {
		R_LOG_DEBUG ("No calling convention for function '%s' to extract register arguments\n", fcn->name);
		return;
	}
	char *fname = r_type_func_guess (anal->sdb_types, fcn->name);
	Sdb *TDB = anal->sdb_types;
	int max_count = r_anal_cc_max_arg (anal, fcn->cc);
	if (!max_count || (*count >= max_count)) {
		free (fname);
		return;
	}
	if (fname) {
		argc = r_type_func_args_count (TDB, fname);
	}

	bool is_call = (op->type & 0xf) == R_ANAL_OP_TYPE_CALL || (op->type & 0xf) == R_ANAL_OP_TYPE_UCALL;
	if (is_call && *count < max_count) {
		RList *callee_rargs_l = NULL;
		int callee_rargs = 0;
		char *callee = NULL;
		ut64 offset = op->jump == UT64_MAX ? op->ptr : op->jump;
		RAnalFunction *f = r_anal_get_function_at (anal, offset);
		if (!f) {
			RCore *core = (RCore *)anal->coreb.core;
			RFlagItem *flag = r_flag_get_by_spaces (core->flags, offset, R_FLAGS_FS_IMPORTS, NULL);
			if (flag) {
				callee = r_type_func_guess (TDB, flag->name);
				if (callee) {
					const char *cc = r_anal_cc_func (anal, callee);
					if (cc && !strcmp (fcn->cc, cc)) {
						callee_rargs = R_MIN (max_count, r_type_func_args_count (TDB, callee));
					}
				}
			}
		} else if (!f->is_variadic && !strcmp (fcn->cc, f->cc)) {
			callee = r_type_func_guess (TDB, f->name);
			if (callee) {
				callee_rargs = R_MIN (max_count, r_type_func_args_count (TDB, callee));
			}
			callee_rargs = callee_rargs
				? callee_rargs
				: r_anal_var_count (anal, f, R_ANAL_VAR_KIND_REG, 1);
			callee_rargs_l = r_anal_var_list (anal, f, R_ANAL_VAR_KIND_REG);
		}
		int i;
		for (i = 0; i < callee_rargs; i++) {
			if (reg_set[i]) {
				continue;
			}
			const char *vname = NULL;
			char *type = NULL;
			char *name = NULL;
			int delta = 0;
			const char *regname = r_anal_cc_arg (anal, fcn->cc, i);
			RRegItem *ri = r_reg_get (anal->reg, regname, -1);
			if (ri) {
				delta = ri->index;
			}
			if (fname) {
				type = r_type_func_args_type (TDB, fname, i);
				vname = r_type_func_args_name (TDB, fname, i);
			}
			if (!vname && callee) {
				type = r_type_func_args_type (TDB, callee, i);
				vname = r_type_func_args_name (TDB, callee, i);
			}
			if (vname) {
				reg_set[i] = 1;
			} else {
				RListIter *it;
				RAnalVar *arg, *found_arg = NULL;
				r_list_foreach (callee_rargs_l, it, arg) {
					if (r_anal_var_get_argnum (arg) == i) {
						found_arg = arg;
						break;
					}
				}
				if (found_arg) {
					type = strdup (found_arg->type);
					vname = name = strdup (found_arg->name);
				}
			}
			if (!vname) {
				name = r_str_newf ("arg%u", (int)i + 1);
				vname = name;
			}
			r_anal_function_set_var (fcn, delta, R_ANAL_VAR_KIND_REG, type, size, true, vname);
			(*count)++;
			free (name);
			free (type);
		}
		free (callee);
		r_list_free (callee_rargs_l);
		free (fname);
		return;
	}

	for (i = 0; i < max_count; i++) {
		const char *regname = r_anal_cc_arg (anal, fcn->cc, i);
		if (regname) {
			int delta = 0;
			RRegItem *ri = NULL;
			RAnalVar *var = NULL;
			bool is_used_like_an_arg = is_used_like_arg (regname, opsreg, opdreg, op, anal);
			if (reg_set[i] != 2 && is_used_like_an_arg) {
				ri = r_reg_get (anal->reg, regname, -1);
				if (ri) {
					delta = ri->index;
				}
			}
			if (reg_set[i] == 1 && is_used_like_an_arg) {
				var = r_anal_function_get_var (fcn, R_ANAL_VAR_KIND_REG, delta);
			} else if (reg_set[i] != 2 && is_used_like_an_arg) {
				const char *vname = NULL;
				char *type = NULL;
				char *name = NULL;
				if ((i < argc) && fname) {
					type = r_type_func_args_type (TDB, fname, i);
					vname = r_type_func_args_name (TDB, fname, i);
				}
				if (!vname) {
					name = r_str_newf ("arg%d", i + 1);
					vname = name;
				}
				var = r_anal_function_set_var (fcn, delta, R_ANAL_VAR_KIND_REG, type, size, true, vname);
				free (name);
				free (type);
				(*count)++;
			} else {
				if (is_reg_in_src (regname, anal, op) || STR_EQUAL (opdreg, regname)) {
					reg_set[i] = 2;
				}
				continue;
			}
			if (is_reg_in_src (regname, anal, op) || STR_EQUAL (regname, opdreg)) {
				reg_set[i] = 1;
			}
			if (var) {
				r_anal_var_set_access (var, var->regname, op->addr, R_ANAL_VAR_ACCESS_TYPE_READ, 0);
				r_meta_set_string (anal, R_META_TYPE_VARTYPE, op->addr, var->name);
			}
		}
	}

	const char *selfreg = r_anal_cc_self (anal, fcn->cc);
	if (selfreg) {
		bool is_used_like_an_arg = is_used_like_arg (selfreg, opsreg, opdreg, op, anal);
		if (reg_set[i] != 2 && is_used_like_an_arg) {
			int delta = 0;
			char *vname = strdup ("self");
			RRegItem *ri = r_reg_get (anal->reg, selfreg, -1);
			if (ri) {
				delta = ri->index;
			}
			RAnalVar *newvar = r_anal_function_set_var (fcn, delta, R_ANAL_VAR_KIND_REG, 0, size, true, vname);
			if (newvar) {
				r_anal_var_set_access (newvar, newvar->regname, op->addr, R_ANAL_VAR_ACCESS_TYPE_READ, 0);
			}
			r_meta_set_string (anal, R_META_TYPE_VARTYPE, op->addr, vname);
			free (vname);
			(*count)++;
		} else {
			if (is_reg_in_src (selfreg, anal, op) || STR_EQUAL (opdreg, selfreg)) {
				reg_set[i] = 2;
			}
		}
		i++;
	}

	const char *errorreg = r_anal_cc_error (anal, fcn->cc);
	if (errorreg) {
		if (reg_set[i] == 0 && STR_EQUAL (opdreg, errorreg)) {
			int delta = 0;
			char *vname = strdup ("error");
			RRegItem *ri = r_reg_get (anal->reg, errorreg, -1);
			if (ri) {
				delta = ri->index;
			}
			RAnalVar *newvar = r_anal_function_set_var (fcn, delta, R_ANAL_VAR_KIND_REG, 0, size, true, vname);
			if (newvar) {
				r_anal_var_set_access (newvar, newvar->regname, op->addr, R_ANAL_VAR_ACCESS_TYPE_READ, 0);
			}
			r_meta_set_string (anal, R_META_TYPE_VARTYPE, op->addr, vname);
			free (vname);
			(*count)++;
			reg_set[i] = 2;
		}
	}
	free (fname);
}