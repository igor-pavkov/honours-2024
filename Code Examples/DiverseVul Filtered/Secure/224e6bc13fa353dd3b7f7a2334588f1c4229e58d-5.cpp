static RBinJavaObj * get_java_bin_obj(RAnal *anal) {
	RBin *b = anal->binb.bin;
	RBinPlugin *plugin = b->cur && b->cur->o ? b->cur->o->plugin : NULL;
	ut8 is_java = (plugin && strcmp (plugin->name, "java") == 0) ? 1 : 0;
	return is_java ? b->cur->o->bin_obj : NULL;
}