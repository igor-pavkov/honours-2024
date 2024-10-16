R_API void *r_bin_free(RBin *bin) {
	if (!bin) {
		return NULL;
	}
	if (bin->io_owned) {
		r_io_free (bin->iob.io);
	}
	bin->file = NULL;
	free (bin->force);
	free (bin->srcdir);
	//r_bin_free_bin_files (bin);
	r_list_free (bin->binfiles);
	r_list_free (bin->binxtrs);
	r_list_free (bin->plugins);
	sdb_free (bin->sdb);
	r_id_pool_free (bin->file_ids);
	memset (bin, 0, sizeof (RBin));
	free (bin);
	return NULL;
}