static inline bool serialize_single_var(RAnalVarProt *vp, RStrBuf *sb) {
	r_return_val_if_fail (vp && sb, false);
	// shouldn't have special chars in them anyways, so replace in place
	sanitize_var_serial (vp->name, false);
	sanitize_var_serial (vp->type, true);
	const char b = vp->isarg? 't': 'f';
	if (!valid_var_kind (vp->kind)) {
		return false;
	}
	return r_strbuf_appendf (sb, "%c%c%d:%s:%s", b, vp->kind, vp->delta, vp->name, vp->type);
}