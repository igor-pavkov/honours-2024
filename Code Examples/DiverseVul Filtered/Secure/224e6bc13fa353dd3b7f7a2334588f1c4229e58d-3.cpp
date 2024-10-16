static void java_update_anal_types (RAnal *anal, RBinJavaObj *bin_obj) {
	Sdb *D = anal->sdb_types;
	if (D && bin_obj) {
		RListIter *iter;
		char *str;
		RList * the_list = r_bin_java_extract_all_bin_type_values (bin_obj);
		if (the_list) {
			r_list_foreach (the_list, iter, str) {
				IFDBG eprintf ("Adding type: %s to known types.\n", str);
				if (str) sdb_set (D, str, "type", 0);
			}
		}
		r_list_free (the_list);
	}
}