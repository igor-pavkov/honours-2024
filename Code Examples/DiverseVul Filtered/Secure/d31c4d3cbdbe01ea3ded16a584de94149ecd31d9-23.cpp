R_API int r_bin_load(RBin *bin, const char *file, ut64 baseaddr, ut64 loadaddr, int xtr_idx, int fd, int rawstr) {
	if (!bin) {
		return false;
	}
	// ALIAS?	return r_bin_load_as (bin, file, baseaddr, loadaddr,
	// xtr_idx, fd, rawstr, 0, file);
	RIOBind *iob = &(bin->iob);
	if (!iob) {
		return false;
	}
	if (!iob->io) {
		iob->io = r_io_new ();	//wtf
		if (!iob->io) {
			return false;
		}
		bin->io_owned = true;
		r_io_bind (iob->io, &bin->iob);		//memleak?
		iob = &bin->iob;
	}
	if (!iob->desc_get (iob->io, fd)) {
		fd = iob->fd_open (iob->io, file, R_IO_READ, 0644);
	}
	bin->rawstr = rawstr;
	// Use the current RIODesc otherwise r_io_map_select can swap them later on
	if (fd < 0) {
		r_io_free (iob->io);
		memset (&bin->iob, 0, sizeof (bin->iob));
		bin->io_owned = false;
		return false;
	}
	//Use the current RIODesc otherwise r_io_map_select can swap them later on
	return r_bin_load_io (bin, fd, baseaddr, loadaddr, xtr_idx);
}