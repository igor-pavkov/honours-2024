static void get_strings_range(RBinFile *arch, RList *list, int min, ut64 from, ut64 to) {
	RBinPlugin *plugin = r_bin_file_cur_plugin (arch);
	RBinString *ptr;
	RListIter *it;

	if (!arch || !arch->buf || !arch->buf->buf) {
		return;
	}
	if (!arch->rawstr) {
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
	if (!to || to > arch->buf->length) {
		to = arch->buf->length;
	}
	if (arch->rawstr != 2) {
		ut64 size = to - from;
		// in case of dump ignore here
		if (arch->rbin->maxstrbuf && size && size > arch->rbin->maxstrbuf) {
			if (arch->rbin->verbose) {
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
	if (string_scan_range (list, arch->buf->buf, min, from, to, -1) < 0) {
		return;
	}
	r_list_foreach (list, it, ptr) {
		RBinSection *s = r_bin_get_section_at (arch->o, ptr->paddr, false);
		if (s) {
			ptr->vaddr = s->vaddr + (ptr->paddr - s->paddr);
		}
	}
}