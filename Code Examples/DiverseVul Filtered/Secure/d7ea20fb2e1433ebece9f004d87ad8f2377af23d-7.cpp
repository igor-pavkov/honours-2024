void r_bin_le_free(r_bin_le_obj_t *bin) {
	r_return_if_fail (bin);
	free (bin->header);
	free (bin->objtbl);
	free (bin->filename);
	free (bin);
}