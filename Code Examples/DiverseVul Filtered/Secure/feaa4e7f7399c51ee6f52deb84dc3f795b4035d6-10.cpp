static int kernelcache_io_read(RIO *io, RIODesc *fd, ut8 *buf, int count) {
	r_return_val_if_fail (io, -1);
	RCore *core = (RCore*) io->corebind.core;

	if (!fd || !core || !core->bin || !core->bin->binfiles) {
		return -1;
	}

	RKernelCacheObj *cache = NULL;
	RListIter *iter;
	RBinFile *bf;
	r_list_foreach (core->bin->binfiles, iter, bf) {
		if (bf->fd == fd->fd && bf->o && bf->o->bin_obj) {
			cache = bf->o->bin_obj;
			if (pending_bin_files) {
				RListIter *to_remove = r_list_contains (pending_bin_files, bf);
				if (to_remove) {
					r_list_delete (pending_bin_files, to_remove);
					if (r_list_empty (pending_bin_files)) {
						r_list_free (pending_bin_files);
						pending_bin_files = NULL;
					}
				}
			}
			break;
		}
	}

	if (!cache) {
		r_list_foreach (pending_bin_files, iter, bf) {
			if (bf->fd == fd->fd && bf->o) {
				cache = bf->o->bin_obj;
				break;
			}
		}
	}

	if (!cache || !cache->original_io_read || cache->rebasing_buffer) {
		if (cache) {
			if ((!cache->rebasing_buffer && fd->plugin->read == &kernelcache_io_read) ||
					(cache->rebasing_buffer && !cache->original_io_read)) {
				return -1;
			}
			if (cache->rebasing_buffer) {
				return cache->original_io_read (io, fd, buf, count);
			}
		}
		if (fd->plugin->read == kernelcache_io_read) {
			if (core->bin->verbose) {
				eprintf ("Avoid recursive reads\n");
			}
			return -1;
		}
		return fd->plugin->read (io, fd, buf, count);
	}

	if (cache->rebase_info) {
		r_rebase_info_populate (cache->rebase_info, cache);
	}

	static ut8 *internal_buffer = NULL;
	static int internal_buf_size = 0;
	if (count > internal_buf_size) {
		if (internal_buffer) {
			R_FREE (internal_buffer);
			internal_buffer = NULL;
		}
		internal_buffer = (ut8 *) malloc (count);
		internal_buf_size = count;
	}

	if (!cache->original_io_read) {
		return -1;
	}
	ut64 io_off = io->off;
	int result = cache->original_io_read (io, fd, internal_buffer, count);

	if (result == count) {
		if (cache->mach0->chained_starts) {
			rebase_buffer_fixup (cache, io_off, fd, internal_buffer, count);
		} else if (cache->rebase_info) {
			rebase_buffer (cache, io_off, fd, internal_buffer, count);
		}
		memcpy (buf, internal_buffer, result);
	}

	return result;
}