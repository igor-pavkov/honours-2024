static void r_anal_var_proto_free(RAnalVarProt *vp) {
	if (vp) {
		free (vp->name);
		free (vp->type);
		free (vp);
	}
}