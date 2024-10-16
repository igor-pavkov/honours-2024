static RList * get_java_bin_obj_list(RAnal *anal) {
	RBinJavaObj *bin_obj = (RBinJavaObj * )get_java_bin_obj(anal);
	// See libr/bin/p/bin_java.c to see what is happening here.  The original intention
	// was to use a shared global db variable from shlr/java/class.c, but the
	// BIN_OBJS_ADDRS variable kept getting corrupted on Mac, so I (deeso) switched the
	// way the access to the db was taking place by using the bin_obj as a proxy back
	// to the BIN_OBJS_ADDRS which is instantiated in libr/bin/p/bin_java.c
	// not the easiest way to make sausage, but its getting made.
	return  r_bin_java_get_bin_obj_list_thru_obj (bin_obj);
}