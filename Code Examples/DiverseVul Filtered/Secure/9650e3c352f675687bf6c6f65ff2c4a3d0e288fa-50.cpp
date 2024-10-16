R_API void r_bin_java_print_local_variable_attr_summary(RBinJavaLocalVariableAttribute *lvattr) {
	if (!lvattr) {
		eprintf ("Attempting to print an invalid RBinJavaLocalVariableAttribute *.\n");
		return;
	}
	Eprintf ("  Local Variable Attribute offset: 0x%08"PFMT64x "\n", lvattr->file_offset);
	Eprintf ("  Local Variable Attribute start_pc: %d\n", lvattr->start_pc);
	Eprintf ("  Local Variable Attribute Length: %d\n", lvattr->length);
	Eprintf ("  Local Variable Attribute name_idx: %d\n", lvattr->name_idx);
	Eprintf ("  Local Variable Attribute name: %s\n", lvattr->name);
	Eprintf ("  Local Variable Attribute descriptor_idx: %d\n", lvattr->descriptor_idx);
	Eprintf ("  Local Variable Attribute descriptor: %s\n", lvattr->descriptor);
	Eprintf ("  Local Variable Attribute index: %d\n", lvattr->index);
}