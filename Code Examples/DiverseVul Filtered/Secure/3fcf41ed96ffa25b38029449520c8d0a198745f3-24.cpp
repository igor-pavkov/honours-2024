R_API RBinFile *r_bin_file_create_append(RBin *bin, const char *file, const ut8 *bytes, ut64 sz, ut64 file_sz, int rawstr, int fd, const char *xtrname, bool steal_ptr) {
	RBinFile *bf = r_bin_file_new (bin, file, bytes, sz, file_sz, rawstr,
				       fd, xtrname, bin->sdb, steal_ptr);
	if (bf) {
		r_list_append (bin->binfiles, bf);
	}
	return bf;
}