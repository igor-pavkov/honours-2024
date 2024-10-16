R_API char *r_bin_java_build_obj_key(RBinJavaObj *bin) {
	char *jvcname = NULL;
	char *cname = r_bin_java_get_this_class_name (bin);
	ut32 class_name_len = cname ? strlen (cname) : strlen ("_unknown_");
	jvcname = malloc (class_name_len + 8 + 30);
	if (cname) {
		snprintf (jvcname, class_name_len + 30, "%d.%s.class", bin->id, cname);
		free (cname);
	} else {
		snprintf (jvcname, class_name_len + 30, "%d._unknown_.class", bin->id);
	}
	return jvcname;
}