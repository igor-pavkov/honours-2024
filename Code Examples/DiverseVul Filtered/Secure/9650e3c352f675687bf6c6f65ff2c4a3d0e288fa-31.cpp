R_API char *r_bin_java_get_item_name_from_cp_item_list(RList *cp_list, RBinJavaCPTypeObj *obj, int depth) {
	/*
	Given a constant poool object Class, FieldRef, MethodRef, or InterfaceMethodRef
	return the actual descriptor string.
	@param cp_list: RList of RBinJavaCPTypeObj *
	@param obj object to look up the name for
	@rvalue ut8* (user frees) or NULL
	*/
	if (obj == NULL || cp_list == NULL || depth < 0) {
		return NULL;
	}
	switch (obj->tag) {
	case R_BIN_JAVA_CP_NAMEANDTYPE:
		return r_bin_java_get_utf8_from_cp_item_list (
			cp_list, obj->info.cp_name_and_type.name_idx);
	case R_BIN_JAVA_CP_CLASS:
		return r_bin_java_get_utf8_from_cp_item_list (
			cp_list, obj->info.cp_class.name_idx);
	// XXX - Probably not good form, but they are the same memory structure
	case R_BIN_JAVA_CP_FIELDREF:
	case R_BIN_JAVA_CP_INTERFACEMETHOD_REF:
	case R_BIN_JAVA_CP_METHODREF:
		obj = r_bin_java_get_item_from_cp_item_list (
			cp_list, obj->info.cp_method.name_and_type_idx);
		return r_bin_java_get_item_name_from_cp_item_list (
			cp_list, obj, depth - 1);
	default:
		return NULL;
	case 0:
		IFDBG eprintf("Invalid 0 tag in the constant pool\n");
		return NULL;
	}
	return NULL;
}