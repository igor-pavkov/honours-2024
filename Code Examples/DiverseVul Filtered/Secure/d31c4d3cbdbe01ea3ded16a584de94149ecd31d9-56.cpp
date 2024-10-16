R_API bool r_bin_load_io_at_offset_as(RBin *bin, int fd, ut64 baseaddr,
		ut64 loadaddr, int xtr_idx, ut64 offset, const char *name) {
	// adding file_sz to help reduce the performance impact on the system
	// in this case the number of bytes read will be limited to 2MB
	// (MIN_LOAD_SIZE)
	// if it fails, the whole file is loaded.
	const ut64 MAX_LOAD_SIZE = 0;  // 0xfffff; //128 * (1 << 10 << 10);
	int res = r_bin_load_io_at_offset_as_sz (bin, fd, baseaddr,
		loadaddr, xtr_idx, offset, name, MAX_LOAD_SIZE);
	if (!res) {
		res = r_bin_load_io_at_offset_as_sz (bin, fd, baseaddr,
			loadaddr, xtr_idx, offset, name, UT64_MAX);
	}
	return res;
}