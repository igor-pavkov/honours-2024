R_API void r_anal_var_list_show(RAnal *anal, RAnalFunction *fcn, int kind, int mode, PJ *pj) {
	RList *list = r_anal_var_list (anal, fcn, kind);
	RAnalVar *var;
	RListIter *iter;
	if (!pj && mode == 'j') {
		return;
	}
	if (mode == 'j') {
		pj_a (pj);
	}
	if (!list) {
		if (mode == 'j') {
			pj_end (pj);
		}
		return;
	}
	//s- at the end of the loop
	if (mode == '*' && !r_list_empty (list)) {
		anal->cb_printf ("s 0x%" PFMT64x "\n", fcn->addr);
	}
	r_list_sort (list, (RListComparator) var_comparator);
	r_list_foreach (list, iter, var) {
		if (var->kind != kind) {
			continue;
		}
		switch (mode) {
		case '*':
			// we can't express all type info here :(
			if (kind == R_ANAL_VAR_KIND_REG) { // registers
				RRegItem *i = r_reg_index_get (anal->reg, var->delta);
				if (!i) {
					eprintf ("Register not found");
					break;
				}
				anal->cb_printf ("\"afv%c %s %s %s\"\n",
					kind, i->name, var->name, var->type);
			} else {
				int delta = kind == R_ANAL_VAR_KIND_BPV
					? var->delta + fcn->bp_off
					: var->delta;
				anal->cb_printf ("\"afv%c %d %s %s\"\n",
					kind, delta, var->name, var->type);
			}
			break;
		case 'j':
			switch (var->kind) {
			case R_ANAL_VAR_KIND_BPV: {
				st64 delta = (st64)var->delta + fcn->bp_off;
				pj_o (pj);
				pj_ks (pj, "name", var->name);
				if (var->isarg) {
					pj_ks (pj, "kind", "arg");
				} else {
					pj_ks (pj, "kind", "var");
				}
				pj_ks (pj, "type", var->type);
				pj_k (pj, "ref");
				pj_o (pj);
				pj_ks (pj, "base", anal->reg->name[R_REG_NAME_BP]);
				pj_kN (pj, "offset", delta);
				pj_end (pj);
				pj_end (pj);
			}
				break;
			case R_ANAL_VAR_KIND_REG: {
				RRegItem *i = r_reg_index_get (anal->reg, var->delta);
				if (!i) {
					eprintf ("Register not found");
					break;
				}
				pj_o (pj);
				pj_ks (pj, "name", var->name);
				pj_ks (pj, "kind", "reg");
				pj_ks (pj, "type", var->type);
				pj_ks (pj, "ref", i->name);
				pj_end (pj);
			}
				break;
			case R_ANAL_VAR_KIND_SPV: {
				st64 delta = (st64)var->delta + fcn->maxstack;
				pj_o (pj);
				pj_ks (pj, "name", var->name);
				if (var->isarg) {
					pj_ks (pj, "kind", "arg");
				} else {				
					pj_ks (pj, "kind", "var");			
				}
				pj_ks (pj, "type", var->type);
				pj_k (pj, "ref");
				pj_o (pj);
				pj_ks (pj, "base", anal->reg->name[R_REG_NAME_SP]);
				pj_kN (pj, "offset", delta);
				pj_end (pj);
				pj_end (pj);
			}
				break;
			}
			break;
		default:
			switch (kind) {
			case R_ANAL_VAR_KIND_BPV:
			{
				int delta = var->delta + fcn->bp_off;
				if (var->isarg) {
					anal->cb_printf ("arg %s %s @ %s+0x%x\n",
						var->type, var->name,
						anal->reg->name[R_REG_NAME_BP],
						delta);
				} else {
					char sign = (-var->delta <= fcn->bp_off) ? '+' : '-';
					anal->cb_printf ("var %s %s @ %s%c0x%x\n",
						var->type, var->name,
						anal->reg->name[R_REG_NAME_BP],
						sign, R_ABS (delta));
				}
			}
				break;
			case R_ANAL_VAR_KIND_REG: {
				RRegItem *i = r_reg_index_get (anal->reg, var->delta);
				if (!i) {
					eprintf ("Register not found");
					break;
				}
				anal->cb_printf ("arg %s %s @ %s\n",
					var->type, var->name, i->name);
				}
				break;
			case R_ANAL_VAR_KIND_SPV:
			{
				int delta = fcn->maxstack + var->delta;
				if (!var->isarg) {
					char sign = (-var->delta <= fcn->maxstack) ? '+' : '-';
					anal->cb_printf ("var %s %s @ %s%c0x%x\n",
						var->type, var->name,
						anal->reg->name[R_REG_NAME_SP],
						sign, R_ABS (delta));
				} else {
					anal->cb_printf ("arg %s %s @ %s+0x%x\n",
						var->type, var->name,
						anal->reg->name[R_REG_NAME_SP],
						delta);

				}
			}
				break;
			}
		}
	}
	if (mode == '*' && !r_list_empty (list)) {
		anal->cb_printf ("s-\n");
	}
	if (mode == 'j') {
		pj_end (pj);
	}
	r_list_free (list);
}