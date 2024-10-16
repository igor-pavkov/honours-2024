R_API int r_bin_load_io_at_offset_as_sz(RBin *bin, int fd, ut64 baseaddr,
		ut64 loadaddr, int xtr_idx, ut64 offset, const char *name, ut64 sz) {
	RIOBind *iob = &(bin->iob);
	RIO *io = iob? iob->io: NULL;
	RListIter *it;
	ut8 *buf_bytes = NULL;
	RBinXtrPlugin *xtr;
	ut64 file_sz = UT64_MAX;
	RBinFile *binfile = NULL;
	int tfd = -1;

	if (!io || (fd < 0) || (st64)sz < 0) {
		return false;
	}
	bool is_debugger = iob->fd_is_dbg (io, fd);
	const char *fname = iob->fd_get_name (io, fd);
	if (loadaddr == UT64_MAX) {
		loadaddr = 0;
	}
	file_sz = iob->fd_size (io, fd);
	// file_sz = UT64_MAX happens when attaching to frida:// and other non-debugger io plugins which results in double opening
	if (is_debugger && file_sz == UT64_MAX) {
		tfd = iob->fd_open (io, fname, R_IO_READ, 0644);
		if (tfd >= 1) {
			file_sz = iob->fd_size (io, tfd);
		}
	}
	if (!sz) {
		sz = file_sz;
	}

	bin->file = fname;
	sz = R_MIN (file_sz, sz);
	if (!r_list_length (bin->binfiles)) {
		if (is_debugger) {
			//use the temporal RIODesc to read the content of the file instead
			//from the memory
			if (tfd >= 0) {
				buf_bytes = calloc (1, sz + 1);
				iob->fd_read_at (io, tfd, 0, buf_bytes, sz);
				// iob->fd_close (io, tfd);
			}
		}
	}
	if (!buf_bytes) {
		buf_bytes = calloc (1, sz + 1);
		if (!buf_bytes) {
			return false;
		}
		ut64 seekaddr = is_debugger? baseaddr: loadaddr;
		if (!iob->fd_read_at (io, fd, seekaddr, buf_bytes, sz)) {
			sz = 0LL;
		}
	}

	if (!name && (st64)sz > 0) {
		// XXX - for the time being this is fine, but we may want to
		// change the name to something like
		// <xtr_name>:<bin_type_name>
		r_list_foreach (bin->binxtrs, it, xtr) {
			if (xtr && xtr->check_bytes (buf_bytes, sz)) {
				if (xtr && (xtr->extract_from_bytes || xtr->extractall_from_bytes)) {
					if (is_debugger && sz != file_sz) {
						R_FREE (buf_bytes);
						if (tfd < 0) {
							tfd = iob->fd_open (io, fname, R_IO_READ, 0);
						}
						sz = iob->fd_size (io, tfd);
						if (sz != UT64_MAX) {
							buf_bytes = calloc (1, sz + 1);
							if (buf_bytes) {
								(void) iob->fd_read_at (io, tfd, 0, buf_bytes, sz);
							}
						}
				//DOUBLECLOSE UAF : iob->fd_close (io, tfd);
						tfd = -1;	// marking it closed
					} else if (sz != file_sz) {
						(void) iob->read_at (io, 0LL, buf_bytes, sz);
					}
					binfile = r_bin_file_xtr_load_bytes (bin, xtr,
						fname, buf_bytes, sz, file_sz,
						baseaddr, loadaddr, xtr_idx,
						fd, bin->rawstr);
				}
				xtr = NULL;
			}
		}
	}
	if (!binfile) {
		bool steal_ptr = true; // transfer buf_bytes ownership to binfile
		binfile = r_bin_file_new_from_bytes (
			bin, fname, buf_bytes, sz, file_sz, bin->rawstr,
			baseaddr, loadaddr, fd, name, NULL, offset, steal_ptr);
	}
	return binfile? r_bin_file_set_cur_binfile (bin, binfile): false;
}