static Sdb *store_versioninfo_gnu_versym(ELFOBJ *bin, Elf_(Shdr) *shdr, int sz) {
	int i;
	const ut64 num_entries = sz / sizeof (Elf_(Versym));
	const char *section_name = "";
	const char *link_section_name = "";
	Elf_(Shdr) *link_shdr = NULL;
	Sdb *sdb = sdb_new0();
	if (!sdb) {
		return NULL;
	}
	if (!bin->version_info[DT_VERSIONTAGIDX (DT_VERSYM)]) {
		sdb_free (sdb);
		return NULL;
	}
	if (shdr->sh_link > bin->ehdr.e_shnum) {
		sdb_free (sdb);
		return NULL;
	}
	link_shdr = &bin->shdr[shdr->sh_link];
	ut8 *edata = (ut8*) calloc (R_MAX (1, num_entries), sizeof (ut16));
	if (!edata) {
		sdb_free (sdb);
		return NULL;
	}
	ut16 *data = (ut16*) calloc (R_MAX (1, num_entries), sizeof (ut16));
	if (!data) {
		free (edata);
		sdb_free (sdb);
		return NULL;
	}
	ut64 off = Elf_(r_bin_elf_v2p) (bin, bin->version_info[DT_VERSIONTAGIDX (DT_VERSYM)]);
	if (bin->shstrtab && shdr->sh_name < bin->shstrtab_size) {
		section_name = &bin->shstrtab[shdr->sh_name];
	}
	if (bin->shstrtab && link_shdr->sh_name < bin->shstrtab_size) {
		link_section_name = &bin->shstrtab[link_shdr->sh_name];
	}
	r_buf_read_at (bin->b, off, edata, sizeof (ut16) * num_entries);
	sdb_set (sdb, "section_name", section_name, 0);
	sdb_num_set (sdb, "num_entries", num_entries, 0);
	sdb_num_set (sdb, "addr", shdr->sh_addr, 0);
	sdb_num_set (sdb, "offset", shdr->sh_offset, 0);
	sdb_num_set (sdb, "link", shdr->sh_link, 0);
	sdb_set (sdb, "link_section_name", link_section_name, 0);
	for (i = num_entries; i--;) {
		data[i] = r_read_ble16 (&edata[i * sizeof (ut16)], bin->endian);
	}
	R_FREE (edata);
	for (i = 0; i < num_entries; i += 4) {
		int j;
		int check_def;
		char key[32] = {0};
		Sdb *sdb_entry = sdb_new0 ();
		snprintf (key, sizeof (key), "entry%d", i / 4);
		sdb_ns_set (sdb, key, sdb_entry);
		sdb_num_set (sdb_entry, "idx", i, 0);

		for (j = 0; (j < 4) && (i + j) < num_entries; ++j) {
			int k;
			char *tmp_val = NULL;
			snprintf (key, sizeof (key), "value%d", j);
			switch (data[i + j]) {
			case 0:
				sdb_set (sdb_entry, key, "0 (*local*)", 0);
				break;
			case 1:
				sdb_set (sdb_entry, key, "1 (*global*)", 0);
				break;
			default:
				tmp_val = sdb_fmt (0, "%x ", data[i+j] & 0x7FFF);
				check_def = true;
				if (bin->version_info[DT_VERSIONTAGIDX (DT_VERNEED)]) {
					Elf_(Verneed) vn;
					ut8 svn[sizeof (Elf_(Verneed))] = {0};
					ut64 offset = Elf_(r_bin_elf_v2p) (bin, bin->version_info[DT_VERSIONTAGIDX (DT_VERNEED)]);
					do {
						Elf_(Vernaux) vna;
						ut8 svna[sizeof (Elf_(Vernaux))] = {0};
						ut64 a_off;
						if (offset > bin->size || offset + sizeof (vn) > bin->size) {
							goto beach;
						}
						if (r_buf_read_at (bin->b, offset, svn, sizeof (svn)) < 0) {
							bprintf ("Warning: Cannot read Verneed for Versym\n");
							goto beach;
						}
						k = 0;
						vn.vn_version = READ16 (svn, k)
						vn.vn_cnt = READ16 (svn, k)
						vn.vn_file = READ32 (svn, k)
						vn.vn_aux = READ32 (svn, k)
						vn.vn_next = READ32 (svn, k)
						a_off = offset + vn.vn_aux;
						do {
							if (a_off > bin->size || a_off + sizeof (vna) > bin->size) {
								goto beach;
							}
							if (r_buf_read_at (bin->b, a_off, svna, sizeof (svna)) < 0) {
								bprintf ("Warning: Cannot read Vernaux for Versym\n");
								goto beach;
							}
							k = 0;
							vna.vna_hash = READ32 (svna, k)
							vna.vna_flags = READ16 (svna, k)
							vna.vna_other = READ16 (svna, k)
							vna.vna_name = READ32 (svna, k)
							vna.vna_next = READ32 (svna, k)
							a_off += vna.vna_next;
						} while (vna.vna_other != data[i + j] && vna.vna_next != 0);

						if (vna.vna_other == data[i + j]) {
							if (vna.vna_name > bin->strtab_size) {
								goto beach;
							}
							sdb_set (sdb_entry, key, sdb_fmt (0, "%s(%s)", tmp_val, bin->strtab + vna.vna_name), 0);
							check_def = false;
							break;
						}
						offset += vn.vn_next;
					} while (vn.vn_next);
				}

				ut64 vinfoaddr = bin->version_info[DT_VERSIONTAGIDX (DT_VERDEF)];
				if (check_def && data[i + j] != 0x8001 && vinfoaddr) {
					Elf_(Verdef) vd;
					ut8 svd[sizeof (Elf_(Verdef))] = {0};
					ut64 offset = Elf_(r_bin_elf_v2p) (bin, vinfoaddr);
					if (offset > bin->size || offset + sizeof (vd) > bin->size) {
						goto beach;
					}
					do {
						if (r_buf_read_at (bin->b, offset, svd, sizeof (svd)) < 0) {
							bprintf ("Warning: Cannot read Verdef for Versym\n");
							goto beach;
						}
						k = 0;
						vd.vd_version = READ16 (svd, k)
						vd.vd_flags = READ16 (svd, k)
						vd.vd_ndx = READ16 (svd, k)
						vd.vd_cnt = READ16 (svd, k)
						vd.vd_hash = READ32 (svd, k)
						vd.vd_aux = READ32 (svd, k)
						vd.vd_next = READ32 (svd, k)
						offset += vd.vd_next;
					} while (vd.vd_ndx != (data[i + j] & 0x7FFF) && vd.vd_next != 0);

					if (vd.vd_ndx == (data[i + j] & 0x7FFF)) {
						Elf_(Verdaux) vda;
						ut8 svda[sizeof (Elf_(Verdaux))] = {0};
						ut64 off_vda = offset - vd.vd_next + vd.vd_aux;
						if (off_vda > bin->size || off_vda + sizeof (vda) > bin->size) {
							goto beach;
						}
						if (r_buf_read_at (bin->b, off_vda, svda, sizeof (svda)) < 0) {
							bprintf ("Warning: Cannot read Verdaux for Versym\n");
							goto beach;
						}
						k = 0;
						vda.vda_name = READ32 (svda, k)
						vda.vda_next = READ32 (svda, k)
						if (vda.vda_name > bin->strtab_size) {
							goto beach;
						}
						const char *name = bin->strtab + vda.vda_name;
						sdb_set (sdb_entry, key, sdb_fmt (0,"%s(%s%-*s)", tmp_val, name, (int)(12 - strlen (name)),")") , 0);
					}
				}
			}
		}
	}
beach:
	free (data);
	return sdb;
}