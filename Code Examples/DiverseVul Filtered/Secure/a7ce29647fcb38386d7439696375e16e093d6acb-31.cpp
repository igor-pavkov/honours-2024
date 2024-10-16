R_API RList *r_anal_var_get_prots(RAnalFunction *fcn) {
	r_return_val_if_fail (fcn, NULL);
	RList *ret = r_list_newf ((RListFree)r_anal_var_proto_free);
	if (ret) {
		void **p;
		r_pvector_foreach (&fcn->vars, p) {
			RAnalVar *var = *p;
			RAnalVarProt *vp = R_NEW0 (RAnalVarProt);
			if (vp) {
				vp->isarg = var->isarg;
				vp->name = strdup (var->name);
				vp->type = strdup (var->type);
				vp->kind = var->kind;
				vp->delta = var->delta;
				r_list_append (ret, vp);
			}
		}
	}
	return ret;
}