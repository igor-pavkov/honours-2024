static int analyze_from_code_attr (RAnal *anal, RAnalFunction *fcn, RBinJavaField *method, ut64 loadaddr) {
	RBinJavaAttrInfo* code_attr = method ? r_bin_java_get_method_code_attribute(method) : NULL;
	ut8 * code_buf = NULL;
	int result = false;
	ut64 code_length = 0;
	ut64 code_addr = -1;

	if (!code_attr) {
		fcn->name = strdup ("sym.UNKNOWN");
		fcn->dsc = strdup ("unknown");
		r_anal_fcn_set_size (NULL, fcn, code_length);
		fcn->type = R_ANAL_FCN_TYPE_FCN;
		fcn->addr = 0;
		return R_ANAL_RET_ERROR;
	}

	code_length = code_attr->info.code_attr.code_length;
	code_addr = code_attr->info.code_attr.code_offset;
	code_buf = calloc (1, code_length);

	anal->iob.read_at (anal->iob.io, code_addr + loadaddr, code_buf, code_length);
	result = analyze_from_code_buffer (anal, fcn, code_addr + loadaddr, code_buf, code_length);
	free (code_buf);

	char *name = strdup (method->name);
	if (name) {
		r_name_filter (name, 80);
		free (fcn->name);
		if (method->class_name) {
			char *cname = strdup (method->class_name);
			r_name_filter (cname, 50);
			fcn->name = r_str_newf ("sym.%s.%s", cname, name);
			free (cname);
		} else {
			fcn->name = r_str_newf ("sym.%s", name);
		}
		free (name);
	}

	free (fcn->dsc);
	fcn->dsc = strdup (method->descriptor);
	IFDBG eprintf ("Completed analysing code from attr, name: %s, desc: %s", fcn->name, fcn->dsc);

	return result;
}