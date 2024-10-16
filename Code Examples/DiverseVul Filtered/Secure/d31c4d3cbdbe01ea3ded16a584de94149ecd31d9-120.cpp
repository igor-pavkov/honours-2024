R_API int r_bin_write_at(RBin *bin, ut64 addr, const ut8 *buf, int size) {
	RIOBind *iob;
	if (!bin || !(iob = &(bin->iob))) {
		return false;
	}
	return iob->write_at (iob->io, addr, buf, size);
}