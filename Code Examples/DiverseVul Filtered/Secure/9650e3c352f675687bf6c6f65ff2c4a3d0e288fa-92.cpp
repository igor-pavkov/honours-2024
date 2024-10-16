R_API RList *r_bin_java_find_cp_const_by_val_utf8(RBinJavaObj *bin_obj, const ut8 *bytes, ut32 len) {
	RList *res = r_list_newf (free);
	ut32 *v = NULL;
	RListIter *iter;
	RBinJavaCPTypeObj *cp_obj;
	IFDBG eprintf("In UTF-8 Looking for %s\n", bytes);
	r_list_foreach (bin_obj->cp_list, iter, cp_obj) {
		if (cp_obj->tag == R_BIN_JAVA_CP_UTF8) {
			IFDBG eprintf("In UTF-8 Looking @ %s\n", cp_obj->info.cp_utf8.bytes);
			IFDBG eprintf("UTF-8 len = %d and memcmp = %d\n", cp_obj->info.cp_utf8.length, memcmp (bytes, cp_obj->info.cp_utf8.bytes, len));
			if (len == cp_obj->info.cp_utf8.length && !memcmp (bytes, cp_obj->info.cp_utf8.bytes, len)) {
				v = malloc (sizeof (ut32));
				if (!v) {
					r_list_free (res);
					return NULL;
				}
				*v = cp_obj->metas->ord;
				IFDBG eprintf("Found a match adding idx: %d\n", *v);
				r_list_append (res, v);
			}
		}
	}
	return res;
}