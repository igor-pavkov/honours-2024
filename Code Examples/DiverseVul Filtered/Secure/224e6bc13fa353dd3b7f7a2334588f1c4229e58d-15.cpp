static int java_analyze_fns_from_buffer( RAnal *anal, ut64 start, ut64 end, int reftype, int depth) {
	int result = R_ANAL_RET_ERROR;
	ut64 addr = start;
	ut64 offset = 0;
	ut64 buf_len = end - start;
	ut8 analyze_all = 0,
		*buffer = NULL;

	if (end == UT64_MAX) {
		//analyze_all = 1;
		buf_len = anal->iob.desc_size (anal->iob.io->desc);

		if (buf_len == UT64_MAX) buf_len = 1024;

		end = start + buf_len;
	}

	buffer = malloc (buf_len);
	if (!buffer) {
		return R_ANAL_RET_ERROR;
	}

	anal->iob.read_at (anal->iob.io, addr, buffer, buf_len);

	while (offset < buf_len) {
		ut64 length = buf_len - offset;

		RAnalFunction *fcn = r_anal_fcn_new ();
		fcn->cc = r_str_const (r_anal_cc_default (anal));
		result = analyze_from_code_buffer ( anal, fcn, addr, buffer+offset, length );
		if (result == R_ANAL_RET_ERROR) {
			eprintf ("Failed to parse java fn: %s @ 0x%04"PFMT64x"\n", fcn->name, fcn->addr);
			// XXX - TO Stop or not to Stop ??
			break;
		}
		//r_listrange_add (anal->fcnstore, fcn);
		r_anal_fcn_tree_insert (&anal->fcn_tree, fcn);
		r_list_append (anal->fcns, fcn);
		offset += r_anal_fcn_size (fcn);
		if (!analyze_all) break;
	}
	free (buffer);
	return result;
}