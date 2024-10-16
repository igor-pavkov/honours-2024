R_API void U(r_bin_java_print_stack_map_append_frame_summary)(RBinJavaStackMapFrame * obj) {
	RListIter *iter, *iter_tmp;
	RList *ptrList;
	RBinJavaVerificationObj *ver_obj;
	Eprintf ("Stack Map Frame Information\n");
	Eprintf ("  Tag Value = 0x%02x Name: %s\n", obj->tag, ((RBinJavaStackMapFrameMetas *) obj->metas->type_info)->name);
	Eprintf ("  Offset: 0x%08"PFMT64x "\n", obj->file_offset);
	Eprintf ("  Local Variable Count = 0x%04x\n", obj->number_of_locals);
	Eprintf ("  Local Variables:\n");
	ptrList = obj->local_items;
	r_list_foreach_safe (ptrList, iter, iter_tmp, ver_obj) {
		r_bin_java_print_verification_info_summary (ver_obj);
	}
	Eprintf ("  Stack Items Count = 0x%04x\n", obj->number_of_stack_items);
	Eprintf ("  Stack Items:\n");
	ptrList = obj->stack_items;
	r_list_foreach_safe (ptrList, iter, iter_tmp, ver_obj) {
		r_bin_java_print_verification_info_summary (ver_obj);
	}
}