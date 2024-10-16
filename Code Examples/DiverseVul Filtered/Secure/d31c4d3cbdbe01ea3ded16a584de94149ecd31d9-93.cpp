R_API int r_bin_reload(RBin *bin, int fd, ut64 baseaddr) {
	RIOBind *iob = &(bin->iob);
	RList *the_obj_list = NULL;
	int res = false;
	RBinFile *bf = NULL;
	ut8 *buf_bytes = NULL;
	ut64 sz = UT64_MAX;

	if (!iob || !iob->io) {
		res = false;
		goto error;
	}
	const char *name = iob->fd_get_name (iob->io, fd);
	bf = r_bin_file_find_by_name (bin, name);
	if (!bf) {
		res = false;
		goto error;
	}

	the_obj_list = bf->objs;

	bf->objs = r_list_newf ((RListFree)r_bin_object_free);
	// invalidate current object reference
	bf->o = NULL;

	sz = iob->fd_size (iob->io, fd);
	if (sz == UT64_MAX || sz > (64 * 1024 * 1024)) { 
		// too big, probably wrong
		eprintf ("Too big\n");
		res = false;
		goto error;
	}
	if (sz == UT64_MAX && iob->fd_is_dbg (iob->io, fd)) {
		// attempt a local open and read
		// This happens when a plugin like debugger does not have a
		// fixed size.
		// if there is no fixed size or its MAXED, there is no way to
		// definitively
		// load the bin-properly.  Many of the plugins require all
		// content and are not
		// stream based loaders
		int tfd = iob->fd_open (iob->io, name, R_IO_READ, 0);
		if (tfd < 0) {
			res = false;
			goto error;
		}
		sz = iob->fd_size (iob->io, tfd);
		if (sz == UT64_MAX) {
			iob->fd_close (iob->io, tfd);
			res = false;
			goto error;
		}
		buf_bytes = calloc (1, sz + 1);
		if (!buf_bytes) {
			iob->fd_close (iob->io, tfd);
			res = false;
			goto error;
		}
		if (!iob->read_at (iob->io, 0LL, buf_bytes, sz)) {
			free (buf_bytes);
			iob->fd_close (iob->io, tfd);
			res = false;
			goto error;
		}
		iob->fd_close (iob->io, tfd);
	} else {
		buf_bytes = calloc (1, sz + 1);
		if (!buf_bytes) {
			res = false;
			goto error;
		}
		if (!iob->fd_read_at (iob->io, fd, 0LL, buf_bytes, sz)) {
			free (buf_bytes);
			res = false;
			goto error;
		}
	}

	bool yes_plz_steal_ptr = true;
	r_bin_file_set_bytes (bf, buf_bytes, sz, yes_plz_steal_ptr);

	if (r_list_length (the_obj_list) == 1) {
		RBinObject *old_o = (RBinObject *)r_list_get_n (the_obj_list, 0);
		res = r_bin_load_io_at_offset_as (bin, fd, baseaddr,
						old_o->loadaddr, 0, old_o->boffset, NULL);
	} else {
		RListIter *iter = NULL;
		RBinObject *old_o;
		r_list_foreach (the_obj_list, iter, old_o) {
			// XXX - naive. do we need a way to prevent multiple "anys" from being opened?
			res = r_bin_load_io_at_offset_as (bin, fd, baseaddr,
				old_o->loadaddr, 0, old_o->boffset, old_o->plugin->name);
		}
	}
	bf->o = r_list_get_n (bf->objs, 0);

error:
	r_list_free (the_obj_list);

	return res;
}