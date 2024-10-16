R_API RList *r_bin_java_get_method_num_name(RBinJavaObj *bin_obj) {
	ut32 i = 0;
	RListIter *iter = NULL;
	RBinJavaField *fm_type;
	RList *res = r_list_newf (free);
	r_list_foreach (bin_obj->methods_list, iter, fm_type) {
		ut32 len = strlen (fm_type->name) + 30;
		char *str = malloc (len);
		snprintf (str, len, "%d %s", i, fm_type->name);
		++i;
		r_list_append (res, str);
	}
	return res;
}