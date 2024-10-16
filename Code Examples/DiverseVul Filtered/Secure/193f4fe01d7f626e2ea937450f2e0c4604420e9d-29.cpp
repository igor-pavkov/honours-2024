static void print_string(RBinFile *bf, RBinString *string, int raw, PJ *pj) {
	r_return_if_fail (bf && string);

	int mode = bf->strmode;
	RBin *bin = bf->rbin;
	if (!bin) {
		return;
	}
	RIO *io = bin->iob.io;
	if (!io) {
		return;
	}
	RBinSection *s = r_bin_get_section_at (bf->o, string->paddr, false);
	if (s) {
		string->vaddr = s->vaddr + (string->paddr - s->paddr);
	}
	const char *section_name = s ? s->name : "";
	const char *type_string = r_bin_string_type (string->type);
	ut64 vaddr = r_bin_get_vaddr (bin, string->paddr, string->vaddr);
	ut64 addr = vaddr;

	// If raw string dump mode, use printf to dump directly to stdout.
	//  PrintfCallback temp = io->cb_printf;
	switch (mode) {
	case R_MODE_JSON:
		{
			if (pj) {
				pj_o (pj);
				pj_kn (pj, "vaddr", vaddr);
				pj_kn (pj, "paddr", string->paddr);
				pj_kn (pj, "ordinal", string->ordinal);
				pj_kn (pj, "size", string->size);
				pj_kn (pj, "length", string->length);
				pj_ks (pj, "section", section_name);
				pj_ks (pj, "type", type_string);
				pj_ks (pj, "string", string->string);
				pj_end (pj);
			}
		}
		break;
	case R_MODE_SIMPLEST:
		io->cb_printf ("%s\n", string->string);
		break;
	case R_MODE_SIMPLE:
		if (raw == 2) {
			io->cb_printf ("0x%08"PFMT64x" %s\n", addr, string->string);
		} else {
			io->cb_printf ("%s\n", string->string);
		}
		break;
	case R_MODE_RADARE: {
		char *f_name = strdup (string->string);
		r_name_filter (f_name, -1);
		if (bin->prefix) {
			io->cb_printf ("f %s.str.%s %u @ 0x%08"PFMT64x"\n"
					"Cs %u @ 0x%08"PFMT64x"\n",
					bin->prefix, f_name, string->size, addr,
					string->size, addr);
		} else {
			io->cb_printf ("f str.%s %u @ 0x%08"PFMT64x"\n"
					"Cs %u @ 0x%08"PFMT64x"\n",
					f_name, string->size, addr,
					string->size, addr);
		}
		free (f_name);
		break;
		}
	case R_MODE_PRINT:
		io->cb_printf ("%03u 0x%08" PFMT64x " 0x%08" PFMT64x " %3u %3u "
			       "(%s) %5s %s\n",
			string->ordinal, string->paddr, vaddr,
			string->length, string->size,
			section_name, type_string, string->string);
		break;
	}
}