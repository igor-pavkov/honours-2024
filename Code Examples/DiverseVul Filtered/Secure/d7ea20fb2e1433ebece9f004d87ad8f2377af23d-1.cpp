RList *r_bin_le_get_relocs(r_bin_le_obj_t *bin) {
	RList *l = r_list_newf ((RListFree)free);
	if (!l) {
		return NULL;
	}
	RList *entries = __get_entries (bin);
	RList *sections = r_bin_le_get_sections (bin);
	LE_image_header *h = bin->header;
	ut64 cur_page = 0;
	const ut64 fix_rec_tbl_off = (ut64)h->frectab + bin->headerOff;
	ut64 offset = fix_rec_tbl_off + r_buf_read_ble32_at (bin->buf, (ut64)h->fpagetab + bin->headerOff + cur_page * sizeof (ut32), h->worder);
	ut64 end = fix_rec_tbl_off + r_buf_read_ble32_at (bin->buf, (ut64)h->fpagetab + bin->headerOff + (cur_page + 1) * sizeof (ut32), h->worder);
	const RBinSection *cur_section = (RBinSection *)r_list_get_n (sections, cur_page);
	ut64 cur_page_offset = cur_section ? cur_section->vaddr : 0;
	while (cur_page < h->mpages) {
		RBinReloc *rel = R_NEW0 (RBinReloc);
		bool rel_appended = false; // whether rel has been appended to l and must not be freed
		if (!rel) {
			break;
		}
		LE_fixup_record_header header;
		int ret = r_buf_read_at (bin->buf, offset, (ut8 *)&header, sizeof (header));
		if (ret != sizeof (header)) {
			eprintf ("Warning: oobread in LE header parsing relocs\n");
			break;
		}
		offset += sizeof (header);
		switch (header.source & F_SOURCE_TYPE_MASK) {
		case BYTEFIXUP:
			rel->type = R_BIN_RELOC_8;
			break;
		case SELECTOR16:
		case OFFSET16:
			rel->type = R_BIN_RELOC_16;
			break;
		case OFFSET32:
		case POINTER32:
		case SELFOFFSET32:
			rel->type = R_BIN_RELOC_32;
			break;
		case POINTER48:
			rel->type = 48;
			break;
		}
		ut64 repeat = 0;
		ut64 source = 0;
		if (header.source & F_SOURCE_LIST) {
			repeat = r_buf_read8_at (bin->buf, offset);
			offset += sizeof (ut8);
		} else {
			source = r_buf_read_ble16_at (bin->buf, offset, h->worder);
			offset += sizeof (ut16);
		}
		ut32 ordinal;
		if (header.target & F_TARGET_ORD16) {
			ordinal = r_buf_read_ble16_at (bin->buf, offset, h->worder);
			offset += sizeof (ut16);
		} else {
			ordinal = r_buf_read8_at (bin->buf, offset);
			offset += sizeof (ut8);
		}
		switch (header.target & F_TARGET_TYPE_MASK) {
		case INTERNAL:
			if ((ordinal - 1) < bin->header->objcnt) {
			rel->addend = bin->objtbl[ordinal - 1].reloc_base_addr;
				if ((header.source & F_SOURCE_TYPE_MASK) != SELECTOR16) {
					if (header.target & F_TARGET_OFF32) {
						rel->addend += r_buf_read_ble32_at (bin->buf, offset, h->worder);
						offset += sizeof (ut32);
					} else {
						rel->addend += r_buf_read_ble16_at (bin->buf, offset, h->worder);
						offset += sizeof (ut16);
					}
				}
			}
			break;
		case IMPORTORD:
		{
			RBinImport *imp = R_NEW0 (RBinImport);
			if (!imp) {
				break;
			}
			char *mod_name = __get_modname_by_ord (bin, ordinal);
			if (!mod_name) {
				r_bin_import_free (imp);
				break;
			}

			if (header.target & F_TARGET_ORD8) {
				ordinal = r_buf_read8_at (bin->buf, offset);
				offset += sizeof (ut8);
			} else if (header.target & F_TARGET_OFF32) {
				ordinal = r_buf_read_ble32_at (bin->buf, offset, h->worder);
				offset += sizeof (ut32);
			} else {
				ordinal = r_buf_read_ble16_at (bin->buf, offset, h->worder);
				offset += sizeof (ut16);
			}
			imp->name = r_str_newf ("%s.%u", mod_name, ordinal);
			imp->ordinal = ordinal;
			rel->import = imp;
			free (mod_name);
			break;
		}
		case IMPORTNAME:
		{
			RBinImport *imp = R_NEW0 (RBinImport);
			if (!imp) {
				break;
			}
			ut32 nameoff;
			if (header.target & F_TARGET_OFF32) {
				nameoff = r_buf_read_ble32_at (bin->buf, offset, h->worder);
				offset += sizeof (ut32);
			} else {
				nameoff = r_buf_read_ble16_at (bin->buf, offset, h->worder);
				offset += sizeof (ut16);
			}
			ut64 off = (ut64)h->impproc + nameoff + bin->headerOff;
			char *proc_name = __read_nonnull_str_at (bin->buf, &off);
			char *mod_name = __get_modname_by_ord (bin, ordinal);
			imp->name = r_str_newf ("%s.%s", r_str_get (mod_name), r_str_get (proc_name));
			rel->import = imp;
			break;
		}
		case INTERNALENTRY:
			rel->addend = (ut64)(size_t)r_list_get_n (entries, ordinal - 1);
			break;
		}
		if (header.target & F_TARGET_ADDITIVE) {
			ut32 additive = 0;
			if (header.target & F_TARGET_ADD32) {
				additive = r_buf_read_ble32_at (bin->buf, offset, h->worder);
				offset += sizeof (ut32);
			} else {
				additive = r_buf_read_ble16_at (bin->buf, offset, h->worder);
				offset += sizeof (ut16);
			}
			rel->addend += additive;
		}
		if (!repeat) {
			rel->vaddr = cur_page_offset + source;
			rel->paddr = cur_section ? cur_section->paddr + source : 0;
			r_list_append (l, rel);
			rel_appended = true;
		}

		if (header.target & F_TARGET_CHAIN) {
			ut32 fixupinfo = r_buf_read_ble32_at (bin->buf, cur_page_offset + source, h->worder);
			ut64 base_target_address = rel->addend - (fixupinfo & 0xFFFFF);
			do {
				fixupinfo = r_buf_read_ble32_at (bin->buf, cur_page_offset + source, h->worder);
				RBinReloc *new = R_NEW0 (RBinReloc);
				*new = *rel;
				new->addend = base_target_address + (fixupinfo & 0xFFFFF);
				r_list_append (l, new);
				source = (fixupinfo >> 20) & 0xFFF;
			} while (source != 0xFFF);
		}

		while (repeat) {
			ut16 off =  r_buf_read_ble16_at (bin->buf, offset, h->worder);
			rel->vaddr = cur_page_offset + off;
			rel->paddr = cur_section ? cur_section->paddr + off : 0;
			RBinReloc *new = R_NEW0 (RBinReloc);
			*new = *rel;
			r_list_append (l, new);
			offset += sizeof (ut16);
			repeat--;
		}
		while (offset >= end) {
			cur_page++;
			if (cur_page >= h->mpages) {
				break;
			}
			ut64 at = h->fpagetab + bin->headerOff;
			ut32 w0 = r_buf_read_ble32_at (bin->buf, at + cur_page * sizeof (ut32), h->worder);
			ut32 w1 = r_buf_read_ble32_at (bin->buf, at + (cur_page + 1) * sizeof (ut32), h->worder);
			offset = fix_rec_tbl_off + w0;
			end = fix_rec_tbl_off + w1;
			if (offset < end) {
				cur_section = (RBinSection *)r_list_get_n (sections, cur_page);
				cur_page_offset = cur_section ? cur_section->vaddr : 0;
			}
		}
		if (!rel_appended) {
			free (rel);
		}
	}
	r_list_free (entries);
	r_list_free (sections);
	return l;
}