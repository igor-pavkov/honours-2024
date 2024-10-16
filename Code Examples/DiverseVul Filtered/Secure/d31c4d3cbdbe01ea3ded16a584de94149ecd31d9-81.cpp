static RBinObject *r_bin_file_object_find_by_id(RBinFile *binfile,
						 ut32 binobj_id) {
	RBinObject *obj;
	RListIter *iter;
	if (binfile)  {
		r_list_foreach (binfile->objs, iter, obj) {
			if (obj->id == binobj_id) {
				return obj;
			}
		}
	}
	return NULL;
}