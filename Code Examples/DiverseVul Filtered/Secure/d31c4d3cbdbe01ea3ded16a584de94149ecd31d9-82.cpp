R_API int r_bin_load_as(RBin *bin, const char *file, ut64 baseaddr,
			 ut64 loadaddr, int xtr_idx, int fd, int rawstr,
			 int fileoffset, const char *name) {
	RIOBind *iob = &(bin->iob);
	if (!iob || !iob->io) {
		return false;
	}
	if (fd < 0) {
		fd = iob->fd_open (iob->io, file, R_IO_READ, 0644);
	}
	if (fd < 0) {
		return false;
	}
	return r_bin_load_io_at_offset_as (bin, fd, baseaddr, loadaddr,
						  xtr_idx, fileoffset, name);
}