static void java_set_function_prototype (RAnal *anal, RAnalFunction *fcn, RBinJavaField *method) {
	RList *the_list = r_bin_java_extract_type_values (method->descriptor);
	Sdb *D = anal->sdb_types;
	Sdb *A = anal->sdb_args;
	const char *type_fmt = "%08"PFMT64x".arg.%d.type",
	     *namek_fmt = "%08"PFMT64x".var.%d.name",
	     *namev_fmt = "%08"PFMT64x"local.%d";

	char  key_buf[1024], value_buf [1024];
	RListIter *iter;
	char *str;

	if (the_list) {
		ut8 start = 0, stop = 0;
		int idx = 0;
		r_list_foreach (the_list, iter, str) {
			IFDBG eprintf ("Adding type: %s to known types.\n", str);
			if (str && *str == '('){
				start = 1;
				continue;
			}

			if (str && start && *str != ')') {
				// set type
				// set arg type
				snprintf (key_buf, sizeof(key_buf)-1, type_fmt, (ut64)fcn->addr, idx);
				sdb_set (A, str, key_buf, 0);
				sdb_set (D, str, "type", 0);
				// set value
				snprintf (key_buf, sizeof(key_buf)-1, namek_fmt, fcn->addr, idx);
				snprintf (value_buf, sizeof(value_buf)-1, namev_fmt, fcn->addr, idx);
				sdb_set (A, value_buf, key_buf, 0);
				idx ++;
			}
			if (start && str && *str == ')') {
				stop = 1;
				continue;
			}

			if ((start & stop & 1) && str) {
				sdb_set (A, str, "ret.type", 0);
				sdb_set (D, str, "type", 0);
			}
		}
		r_list_free (the_list);
	}
}