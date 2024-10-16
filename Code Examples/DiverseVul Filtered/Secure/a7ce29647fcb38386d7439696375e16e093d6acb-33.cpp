R_API char *r_anal_var_get_constraints_readable(RAnalVar *var) {
	size_t n = var->constraints.len;
	if (!n) {
		return NULL;
	}
	bool low = false, high = false;
	RStrBuf sb;
	r_strbuf_init (&sb);
	size_t i;
	for (i = 0; i < n; i += 1) {
		RAnalVarConstraint *constr = r_vector_index_ptr (&var->constraints, i);
		switch (constr->cond) {
		case R_ANAL_COND_LE:
			if (high) {
				r_strbuf_append (&sb, " && ");
			}
			r_strbuf_appendf (&sb, "<= 0x%"PFMT64x "", constr->val);
			low = true;
			break;
		case R_ANAL_COND_LT:
			if (high) {
				r_strbuf_append (&sb, " && ");
			}
			r_strbuf_appendf (&sb, "< 0x%"PFMT64x "", constr->val);
			low = true;
			break;
		case R_ANAL_COND_GE:
			r_strbuf_appendf (&sb, ">= 0x%"PFMT64x "", constr->val);
			high = true;
			break;
		case R_ANAL_COND_GT:
			r_strbuf_appendf (&sb, "> 0x%"PFMT64x "", constr->val);
			high = true;
			break;
		default:
			break;
		}
		if (low && high && i != n - 1) {
			r_strbuf_append (&sb, " || ");
			low = false;
			high = false;
		}
	}
	return r_strbuf_drain_nofree (&sb);
}