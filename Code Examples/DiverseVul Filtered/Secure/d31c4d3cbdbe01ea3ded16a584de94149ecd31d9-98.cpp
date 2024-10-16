R_API int r_bin_read_at(RBin *bin, ut64 addr, ut8 *buf, int size) {
	RIOBind *iob;
	if (!bin || !(iob = &(bin->iob))) {
		return false;
	}
	return iob->read_at (iob->io, addr, buf, size);
}