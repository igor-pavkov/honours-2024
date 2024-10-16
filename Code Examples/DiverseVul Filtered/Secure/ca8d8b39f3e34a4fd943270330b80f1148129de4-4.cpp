void MACH0_(kv_loadlibs)(struct MACH0_(obj_t) *bin) {
	int i;
	char lib_flagname[128];
	for (i = 0; i < bin->nlibs; i++) {
		snprintf (lib_flagname, sizeof (lib_flagname), "libs.%d.name", i);
		sdb_set (bin->kv, lib_flagname, bin->libs[i], 0);
	}
}