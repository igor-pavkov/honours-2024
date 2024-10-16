R_API RBinJavaStackMapFrame *r_bin_java_build_stack_frame_from_local_variable_table(RBinJavaObj *bin, RBinJavaAttrInfo *attr) {
	RBinJavaStackMapFrame *sf = r_bin_java_default_stack_frame ();
	RBinJavaLocalVariableAttribute *lvattr = NULL;
	RBinJavaVerificationObj *type_item;
	RListIter *iter = NULL;
	ut32 value_cnt = 0;
	ut8 value;
	if (!sf || !bin || !attr || attr->type != R_BIN_JAVA_ATTR_TYPE_LOCAL_VARIABLE_TABLE_ATTR) {
		eprintf ("Attempting to create a stack_map frame from a bad attribute.\n");
		return sf;
	}
	sf->number_of_locals = attr->info.local_variable_table_attr.table_length;
	r_list_foreach (attr->info.local_variable_table_attr.local_variable_table, iter, lvattr) {
		ut32 pos = 0;
		// knock the array Types
		while (lvattr->descriptor[pos] == '[') {
			pos++;
		}
		value = lvattr->descriptor[pos];
		// IFDBG eprintf ("Found the following type value: %c at pos %d in %s\n", value, pos, lvattr->descriptor);
		switch (value) {
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			type_item = r_bin_java_verification_info_from_type (bin, R_BIN_JAVA_STACKMAP_INTEGER, 0);
			break;
		case 'F':
			type_item = r_bin_java_verification_info_from_type (bin, R_BIN_JAVA_STACKMAP_FLOAT, 0);
			break;
		case 'D':
			type_item = r_bin_java_verification_info_from_type (bin, R_BIN_JAVA_STACKMAP_DOUBLE, 0);
			break;
		case 'J':
			type_item = r_bin_java_verification_info_from_type (bin, R_BIN_JAVA_STACKMAP_LONG, 0);
			break;
		case 'L':
			// TODO: FIXME write something that will iterate over the CP Pool and find the
			// CONSTANT_Class_info referencing this
		{
			ut16 idx = r_bin_java_find_cp_class_ref_from_name_idx (bin, lvattr->name_idx);
			type_item = r_bin_java_verification_info_from_type (bin, R_BIN_JAVA_STACKMAP_OBJECT, idx);
		}
		break;
		default:
			eprintf ("r_bin_java_build_stack_frame_from_local_variable_table: "
				"not sure how to handle: name: %s, type: %s\n", lvattr->name, lvattr->descriptor);
			type_item = r_bin_java_verification_info_from_type (bin, R_BIN_JAVA_STACKMAP_NULL, 0);
		}
		if (type_item) {
			r_list_append (sf->local_items, (void *) type_item);
		}
		value_cnt++;
	}
	if (value_cnt != attr->info.local_variable_table_attr.table_length) {
		IFDBG eprintf("r_bin_java_build_stack_frame_from_local_variable_table: "
		"Number of locals not accurate.  Expected %d but got %d",
		attr->info.local_variable_table_attr.table_length, value_cnt);
	}
	return sf;
}