R_API char *r_anal_var_prot_serialize(RList *l, bool spaces) {
	r_return_val_if_fail (l, NULL);
	if (l->length == 0) {
		return NULL;
	}

	RStrBuf *sb = r_strbuf_new ("");
	if (!sb) {
		return NULL;
	}
	r_strbuf_reserve (sb, r_list_length (l) * 0x10);

	char *sep = spaces? ", ": ",";
	size_t len = strlen (sep);
	RAnalVarProt *v;
	RAnalVarProt *top = (RAnalVarProt *)r_list_get_top (l);
	RListIter *iter;
	r_list_foreach (l, iter, v) {
		if (!serialize_single_var (v, sb) || (v != top && !r_strbuf_append_n (sb, sep, len))) {
			r_strbuf_free (sb);
			return NULL;
		}
	}
	return r_strbuf_drain (sb);
}