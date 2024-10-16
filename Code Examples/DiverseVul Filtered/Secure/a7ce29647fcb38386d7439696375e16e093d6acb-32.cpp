R_API char *r_anal_function_format_sig(R_NONNULL RAnal *anal, R_NONNULL RAnalFunction *fcn, R_NULLABLE char *fcn_name,
		R_NULLABLE RAnalFcnVarsCache *reuse_cache, R_NULLABLE const char *fcn_name_pre, R_NULLABLE const char *fcn_name_post) {
	RAnalFcnVarsCache *cache = NULL;

	if (!fcn_name) {
		fcn_name = fcn->name;
		if (!fcn_name) {
			return NULL;
		}
	}

	RStrBuf *buf = r_strbuf_new (NULL);
	if (!buf) {
		return NULL;
	}

	Sdb *TDB = anal->sdb_types;
	char *type_fcn_name = r_type_func_guess (TDB, fcn_name);
	if (type_fcn_name && r_type_func_exist (TDB, type_fcn_name)) {
		const char *fcn_type = r_type_func_ret (anal->sdb_types, type_fcn_name);
		if (R_STR_ISNOTEMPTY (fcn_type)) {
			const char *sp = " ";
			if (*fcn_type && (fcn_type[strlen (fcn_type) - 1] == '*')) {
				sp = "";
			}
			r_strbuf_appendf (buf, "%s%s", fcn_type, sp);
		}
	}

	if (fcn_name_pre) {
		r_strbuf_append (buf, fcn_name_pre);
	}
	r_strbuf_append (buf, fcn_name);
	if (fcn_name_post) {
		r_strbuf_append (buf, fcn_name_post);
	}
	r_strbuf_append (buf, " (");

	if (type_fcn_name && r_type_func_exist (TDB, type_fcn_name)) {
		int i, argc = r_type_func_args_count (TDB, type_fcn_name);
		bool comma = true;
		// This avoids false positives present in argument recovery
		// and straight away print arguments fetched from types db
		for (i = 0; i < argc; i++) {
			char *type = r_type_func_args_type (TDB, type_fcn_name, i);
			const char *name = r_type_func_args_name (TDB, type_fcn_name, i);
			if (!type || !*type || !name) {
				eprintf ("Missing type for %s\n", type_fcn_name);
				goto beach;
			}
			if (i == argc - 1) {
				comma = false;
			}
			size_t len = strlen (type);
			const char *tc = len > 0 && type[len - 1] == '*'? "": " ";
			r_strbuf_appendf (buf, "%s%s%s%s", type, tc, name, comma? ", ": "");
			free (type);
		}
		goto beach;
	}
	R_FREE (type_fcn_name);


	cache = reuse_cache;
	if (!cache) {
		cache = R_NEW0 (RAnalFcnVarsCache);
		if (!cache) {
			type_fcn_name = NULL;
			goto beach;
		}
		r_anal_function_vars_cache_init (anal, cache, fcn);
	}

	bool comma = true;
	bool arg_bp = false;
	size_t tmp_len;
	RAnalVar *var;
	RListIter *iter;

	r_list_foreach (cache->rvars, iter, var) {
		// assume self, error are always the last
		if (!strcmp (var->name, "self") || !strcmp (var->name, "error")) {
			r_strbuf_slice (buf, 0, r_strbuf_length (buf) - 2);
			break;
		}
		tmp_len = strlen (var->type);
		if (tmp_len > 0) {
			r_strbuf_appendf (buf, "%s%s%s%s", var->type,
				tmp_len && var->type[tmp_len - 1] == '*' ? "" : " ",
				var->name, iter->n ? ", " : "");
		}
	}

	r_list_foreach (cache->bvars, iter, var) {
		if (var->isarg) {
			if (!r_list_empty (cache->rvars) && comma) {
				r_strbuf_append (buf, ", ");
				comma = false;
			}
			arg_bp = true;
			tmp_len = strlen (var->type);
			if (tmp_len > 0) {
				r_strbuf_appendf (buf, "%s%s%s%s", var->type,
						tmp_len && var->type[tmp_len - 1] =='*' ? "" : " ",
						var->name, iter->n ? ", " : "");
			}
		}
	}

	comma = true;
	const char *maybe_comma = ", ";
	r_list_foreach (cache->svars, iter, var) {
		if (var->isarg) {
			if (!*maybe_comma || ((arg_bp || !r_list_empty (cache->rvars)) && comma)) {
				comma = false;
				r_strbuf_append (buf, ", ");
			}
			tmp_len = strlen (var->type);
			if (iter->n && ((RAnalVar *)iter->n->data)->isarg) {
				maybe_comma = ", ";
			} else {
				maybe_comma = "";
			}
			if (tmp_len > 0) {
				r_strbuf_appendf (buf, "%s%s%s%s", var->type,
					tmp_len && var->type[tmp_len - 1] == '*'? "": " ",
					var->name, maybe_comma);
			}
		}
	}

beach:
	r_strbuf_append (buf, ");");
	R_FREE (type_fcn_name);
	if (!reuse_cache) {
		// !reuse_cache => we created our own cache
		r_anal_function_vars_cache_fini (cache);
		free (cache);
	}
	return r_strbuf_drain (buf);
}