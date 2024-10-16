static void print_string(RBinString *string, RBinFile *bf) {
	if (!string || !bf) {
		return;
	}
	int mode = bf->strmode;
	ut64 addr , vaddr;
	RBin *bin = bf->rbin;
	const char *section_name, *type_string;
	RIO *io = bin->iob.io;
	if (!io) {
		return;
	}
	RBinSection *s = r_bin_get_section_at (bf->o, string->paddr, false);
	if (s) {
		string->vaddr = s->vaddr + (string->paddr - s->paddr);
	}
	section_name = s ? s->name : "";
	type_string = r_bin_string_type (string->type);
	vaddr = addr = r_bin_get_vaddr (bin, string->paddr, string->vaddr);

	switch(mode) {
	case MODE_SIMPLE :
		io->cb_printf ("0x%08" PFMT64x " %s\n", addr, string->string);
		break;
	case MODE_RADARE :
		{
		char *f_name, *nstr;
		f_name = strdup (string->string);
		r_name_filter (f_name, 512);
		if (bin->prefix) {
			nstr = r_str_newf ("%s.str.%s", bin->prefix, f_name);
			io->cb_printf ("f %s.str.%s %"PFMT64d" @ 0x%08"PFMT64x"\n"
					"Cs %"PFMT64d" @ 0x%08"PFMT64x"\n",
					bin->prefix, f_name, string->size, addr,
					string->size, addr);
		} else {
			nstr = r_str_newf ("str.%s", f_name);
			io->cb_printf ("f str.%s %"PFMT64d" @ 0x%08"PFMT64x"\n"
					"Cs %"PFMT64d" @ 0x%08"PFMT64x"\n",
					f_name, string->size, addr,
					string->size, addr);
		}
		free (nstr);
		free (f_name);
		break;
		}
	case MODE_PRINT :
		io->cb_printf ("%03u 0x%08"PFMT64x" 0x%08"
				PFMT64x" %3u %3u "
				"(%s) %5s %s\n",
				string->ordinal, string->paddr, vaddr,
				string->length, string->size,
				section_name, type_string, string->string);
		break;
	}
}