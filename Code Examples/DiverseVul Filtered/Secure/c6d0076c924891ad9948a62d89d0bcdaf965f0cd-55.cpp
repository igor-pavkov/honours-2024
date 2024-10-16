RBinElfReloc* Elf_(r_bin_elf_get_relocs)(ELFOBJ *bin) {
	int res, rel, rela, i, j;
	size_t reloc_num = 0;
	RBinElfReloc *ret = NULL;

	if (!bin || !bin->g_sections) {
		return NULL;
	}
	reloc_num = get_relocs_num (bin);
	if (!reloc_num)	{
		return NULL;
	}
	bin->reloc_num = reloc_num;
	ret = (RBinElfReloc*)calloc ((size_t)reloc_num + 1, sizeof(RBinElfReloc));
	if (!ret) {
		return NULL;
	}
#if DEAD_CODE
	ut64 section_text_offset = Elf_(r_bin_elf_get_section_offset) (bin, ".text");
	if (section_text_offset == -1) {
		section_text_offset = 0;
	}
#endif
	for (i = 0, rel = 0; !bin->g_sections[i].last && rel < reloc_num ; i++) {
		bool is_rela = 0 == strncmp (bin->g_sections[i].name, ".rela.", strlen (".rela."));
		bool is_rel  = 0 == strncmp (bin->g_sections[i].name, ".rel.",  strlen (".rel."));
		if (!is_rela && !is_rel) {
			continue;
		}
		for (j = 0; j < bin->g_sections[i].size; j += res) {
			if (bin->g_sections[i].size > bin->size) {
				break;
			}
			if (bin->g_sections[i].offset > bin->size) {
				break;
			}
			if (rel >= reloc_num) {
				bprintf ("Internal error: ELF relocation buffer too small,"
				         "please file a bug report.");
				break;
			}
			if (!bin->is_rela) {
				rela = is_rela? DT_RELA : DT_REL;
			} else {
				rela = bin->is_rela;
			}
			res = read_reloc (bin, &ret[rel], rela, bin->g_sections[i].offset + j);
			if (j + res > bin->g_sections[i].size) {
				bprintf ("Warning: malformed file, relocation entry #%u is partially beyond the end of section %u.\n", rel, i);
			}
			if (bin->ehdr.e_type == ET_REL) {
				if (bin->g_sections[i].info < bin->ehdr.e_shnum && bin->shdr) {
					ret[rel].rva = bin->shdr[bin->g_sections[i].info].sh_offset + ret[rel].offset;
					ret[rel].rva = Elf_(r_bin_elf_p2v) (bin, ret[rel].rva);
				} else {
					ret[rel].rva = ret[rel].offset;
				}
			} else {
				ret[rel].rva = ret[rel].offset;
				ret[rel].offset = Elf_(r_bin_elf_v2p) (bin, ret[rel].offset);
			}
			ret[rel].last = 0;
			if (res < 0) {
				break;
			}
			rel++;
		}
	}
	ret[reloc_num].last = 1;
	return ret;
}