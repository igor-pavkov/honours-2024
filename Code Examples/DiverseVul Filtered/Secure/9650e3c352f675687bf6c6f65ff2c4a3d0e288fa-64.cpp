R_API RList *r_bin_java_extract_all_bin_type_values(RBinJavaObj *bin_obj) {
	RListIter *fm_type_iter;
	RList *all_types = r_list_new ();
	RBinJavaField *fm_type;
	// get all field types
	r_list_foreach (bin_obj->fields_list, fm_type_iter, fm_type) {
		char *desc = NULL;
		if (!extract_type_value (fm_type->descriptor, &desc)) {
			return NULL;
		}
		IFDBG eprintf("Adding field type: %s\n", desc);
		r_list_append (all_types, desc);
	}
	// get all method types
	r_list_foreach (bin_obj->methods_list, fm_type_iter, fm_type) {
		RList *the_list = r_bin_java_extract_type_values (fm_type->descriptor);
		RListIter *desc_iter;
		char *str;
		r_list_foreach (the_list, desc_iter, str) {
			if (str && *str != '(' && *str != ')') {
				r_list_append (all_types, strdup (str));
				IFDBG eprintf("Adding method type: %s\n", str);
			}
		}
		r_list_free (the_list);
	}
	return all_types;
}