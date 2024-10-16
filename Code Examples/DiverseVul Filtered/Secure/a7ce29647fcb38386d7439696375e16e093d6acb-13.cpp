R_API bool r_anal_var_display(RAnal *anal, RAnalVar *var) {
	char *fmt = r_type_format (anal->sdb_types, var->type);
	RRegItem *i;
	if (!fmt) {
		eprintf ("type:%s doesn't exist\n", var->type);
		return false;
	}
	bool usePxr = !strcmp (var->type, "int"); // hacky but useful
	switch (var->kind) {
	case R_ANAL_VAR_KIND_REG:
		i = r_reg_index_get (anal->reg, var->delta);
		if (i) {
			if (usePxr) {
				anal->cb_printf ("pxr $w @r:%s\n", i->name);
			} else {
				anal->cb_printf ("pf r (%s)\n", i->name);
			}
		} else {
			eprintf ("register not found\n");
		}
		break;
	case R_ANAL_VAR_KIND_BPV: {
		const st32 real_delta = var->delta + var->fcn->bp_off;
		const ut32 udelta = R_ABS (real_delta);
		const char sign = real_delta >= 0 ? '+' : '-';
		if (usePxr) {
			anal->cb_printf ("pxr $w @%s%c0x%x\n", anal->reg->name[R_REG_NAME_BP], sign, udelta);
		} else {
			anal->cb_printf ("pf %s @%s%c0x%x\n", fmt, anal->reg->name[R_REG_NAME_BP], sign, udelta);
		}
	}
		break;
	case R_ANAL_VAR_KIND_SPV: {
		ut32 udelta = R_ABS (var->delta + var->fcn->maxstack);
		if (usePxr) {
			anal->cb_printf ("pxr $w @%s+0x%x\n", anal->reg->name[R_REG_NAME_SP], udelta);
		} else {
			anal->cb_printf ("pf %s @ %s+0x%x\n", fmt, anal->reg->name[R_REG_NAME_SP], udelta);
		}
		break;
	}
	}
	free (fmt);
	return true;
}