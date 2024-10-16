R_API void r_bin_list_archs(RBin *bin, int mode) {
	RListIter *iter;
	int i = 0;
	char unk[128];
	char archline[128];
	RBinFile *binfile = r_bin_cur (bin);
	RBinObject *obj = NULL;
	const char *name = binfile? binfile->file: NULL;
	int narch = binfile? binfile->narch: 0;

	//are we with xtr format?
	if (binfile && binfile->curxtr) {
		list_xtr_archs (bin, mode);
		return;
	}
	Sdb *binfile_sdb = binfile? binfile->sdb: NULL;
	if (!binfile_sdb) {
		eprintf ("Cannot find SDB!\n");
		return;
	} else if (!binfile) {
		eprintf ("Binary format not currently loaded!\n");
		return;
	}
	sdb_unset (binfile_sdb, ARCHS_KEY, 0);
	if (mode == 'j') {
		bin->cb_printf ("\"bins\":[");
	}
	RBinFile *nbinfile = r_bin_file_find_by_name_n (bin, name, i);
	if (!nbinfile) {
		return;
	}
	i = -1;
	r_list_foreach (nbinfile->objs, iter, obj) {
		RBinInfo *info = obj->info;
		char bits = info? info->bits: 0;
		ut64 boffset = obj->boffset;
		ut32 obj_size = obj->obj_size;
		const char *arch = info? info->arch: NULL;
		const char *machine = info? info->machine: "unknown_machine";

		i++;
		if (!arch) {
			snprintf (unk, sizeof (unk), "unk_%d", i);
			arch = unk;
		}

		if (info && narch > 1) {
			switch (mode) {
			case 'q':
				bin->cb_printf ("%s\n", arch);
				break;
			case 'j':
				bin->cb_printf ("%s{\"arch\":\"%s\",\"bits\":%d,"
						"\"offset\":%" PFMT64d ",\"size\":%d,"
						"\"machine\":\"%s\"}",
						i? ",": "", arch, bits,
						boffset, obj_size, machine);
				break;
			default:
				bin->cb_printf ("%03i 0x%08" PFMT64x " %d %s_%i %s\n", i,
						boffset, obj_size, arch, bits, machine);
			}
			snprintf (archline, sizeof (archline) - 1,
				"0x%08" PFMT64x ":%d:%s:%d:%s",
				boffset, obj_size, arch, bits, machine);
			/// xxx machine not exported?
			//sdb_array_push (binfile_sdb, ARCHS_KEY, archline, 0);
		} else {
			if (info) {
				switch (mode) {
				case 'q':
					bin->cb_printf ("%s\n", arch);
					break;
				case 'j':
					bin->cb_printf ("%s{\"arch\":\"%s\",\"bits\":%d,"
							"\"offset\":%" PFMT64d ",\"size\":%d,"
							"\"machine\":\"%s\"}",
							i? ",": "", arch, bits,
							boffset, obj_size, machine);
					break;
				default:
					bin->cb_printf ("%03i 0x%08" PFMT64x " %d %s_%d\n", i,
							boffset, obj_size, arch, bits);
				}
				snprintf (archline, sizeof (archline),
					"0x%08" PFMT64x ":%d:%s:%d",
					boffset, obj_size, arch, bits);
			} else if (nbinfile && mode) {
				switch (mode) {
				case 'q':
					bin->cb_printf ("%s\n", arch);
					break;
				case 'j':
					bin->cb_printf ("%s{\"arch\":\"unk_%d\",\"bits\":%d,"
							"\"offset\":%" PFMT64d ",\"size\":%d,"
							"\"machine\":\"%s\"}",
							i? ",": "", i, bits,
							boffset, obj_size, machine);
					break;
				default:
					bin->cb_printf ("%03i 0x%08" PFMT64x " %d unk_0\n", i,
							boffset, obj_size);
				}
				snprintf (archline, sizeof (archline),
					"0x%08" PFMT64x ":%d:%s:%d",
					boffset, obj_size, "unk", 0);
			} else {
				eprintf ("Error: Invalid RBinFile.\n");
			}
			//sdb_array_push (binfile_sdb, ARCHS_KEY, archline, 0);
		}
	}
	if (mode == 'j') {
		bin->cb_printf ("]");
	}
}