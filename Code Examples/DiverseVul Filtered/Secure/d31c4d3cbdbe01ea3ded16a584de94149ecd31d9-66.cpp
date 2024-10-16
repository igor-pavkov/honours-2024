R_API void r_bin_force_plugin(RBin *bin, const char *name) {
	free (bin->force);
	bin->force = (name && *name)? strdup (name): NULL;
}