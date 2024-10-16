static void r_bin_object_free(void /*RBinObject*/ *o_) {
	RBinObject *o = o_;
	if (!o) {
		return;
	}
	r_bin_info_free (o->info);
	r_bin_object_delete_items (o);
	R_FREE (o);
}