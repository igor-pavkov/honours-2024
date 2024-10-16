R_API char *r_bin_java_get_desc_from_cp_item_list(RList *cp_list, ut64 idx) {
	/*
	Given a constant poool object FieldRef, MethodRef, or InterfaceMethodRef
	return the actual descriptor string.
	@rvalue ut8* (user frees) or NULL
	*/
	RBinJavaCPTypeObj *obj = r_bin_java_get_item_from_cp_item_list (cp_list, idx);
	if (cp_list == NULL) {
		return NULL;
	}
	return r_bin_java_get_item_desc_from_cp_item_list (cp_list, obj, MAX_CPITEMS);
}