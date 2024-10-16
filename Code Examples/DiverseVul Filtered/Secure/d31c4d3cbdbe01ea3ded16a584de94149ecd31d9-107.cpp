R_API int r_bin_load_io(RBin *bin, int fd, ut64 baseaddr, ut64 loadaddr, int xtr_idx) {
	return r_bin_load_io_at_offset_as (bin, fd, baseaddr, loadaddr, xtr_idx, 0, NULL);
}