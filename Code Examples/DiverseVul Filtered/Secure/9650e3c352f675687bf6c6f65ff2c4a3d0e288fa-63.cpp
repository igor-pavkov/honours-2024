R_API RBinJavaCPTypeObj *r_bin_java_clone_cp_item(RBinJavaCPTypeObj *obj) {
	RBinJavaCPTypeObj *clone_obj = NULL;
	if (obj == NULL) {
		return clone_obj;
	}
	clone_obj = R_NEW0 (RBinJavaCPTypeObj);
	if (clone_obj) {
		memcpy (clone_obj, obj, sizeof (RBinJavaCPTypeObj));
		clone_obj->metas = (RBinJavaMetaInfo *) R_NEW0 (RBinJavaMetaInfo);
		clone_obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[clone_obj->tag];
		clone_obj->name = strdup (obj->name? obj->name: "unk");
		if (obj->tag == R_BIN_JAVA_CP_UTF8) {
			clone_obj->info.cp_utf8.bytes = (ut8 *) malloc (obj->info.cp_utf8.length + 1);
			if (clone_obj->info.cp_utf8.bytes) {
				memcpy (clone_obj->info.cp_utf8.bytes, obj->info.cp_utf8.bytes, clone_obj->info.cp_utf8.length);
			} else {
				// TODO: eprintf allocation error
			}
		}
	}
	return clone_obj;
}