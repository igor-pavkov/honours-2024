R_API void r_bin_file_get_strings_range(RBinFile *bf, RList *list, int min, ut64 from, ut64 to) {
	RBinPlugin *plugin = r_bin_file_cur_plugin (bf);
	RBinString *ptr;
	RListIter *it;

	if (!bf || !bf->buf) {
		return;
	}
	if (!bf->rawstr) {
		if (!plugin || !plugin->info) {
			return;
		}
	}
	if (!min) {
		min = plugin? plugin->minstrlen: 4;
	}
	/* Some plugins return zero, fix it up */
	if (!min) {
		min = 4;
	}
	if (min < 0) {
		return;
	}
	if (!to || to > bf->buf->length) {
		to = r_buf_size (bf->buf);
	}
	if (!to) {
		eprintf ("Empty file with fd %d?\n", bf->buf->fd);
		return;
	}
	if (bf->rawstr != 2) {
		ut64 size = to - from;
		// in case of dump ignore here
		if (bf->rbin->maxstrbuf && size && size > bf->rbin->maxstrbuf) {
			if (bf->rbin->verbose) {
				eprintf ("WARNING: bin_strings buffer is too big "
					"(0x%08" PFMT64x
					")."
					" Use -zzz or set bin.maxstrbuf "
					"(RABIN2_MAXSTRBUF) in r2 (rabin2)\n",
					size);
			}
			return;
		}
	}
	if (string_scan_range (list, bf, min, from, to, -1) < 0) {
		return;
	}
	r_list_foreach (list, it, ptr) {
		RBinSection *s = r_bin_get_section_at (bf->o, ptr->paddr, false);
		if (s) {
			ptr->vaddr = s->vaddr + (ptr->paddr - s->paddr);
		}
	}
}