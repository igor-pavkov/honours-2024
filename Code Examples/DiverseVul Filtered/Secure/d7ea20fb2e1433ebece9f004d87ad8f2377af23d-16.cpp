RList *__get_entries(r_bin_le_obj_t *bin) {
	ut64 offset = (ut64)bin->header->enttab + bin->headerOff;
	RList *l = r_list_newf (free);
	if (!l) {
		return NULL;
	}
	while (true) {
		LE_entry_bundle_header header;
		LE_entry_bundle_entry e;
		r_buf_read_at (bin->buf, offset, (ut8 *)&header, sizeof (header));
		if (!header.count) {
			break;
		}
		if ((header.type & ~ENTRY_PARAMETER_TYPING_PRESENT) == UNUSED_ENTRY) {
			offset += sizeof (header.type) + sizeof (header.count);
			while (header.count) {
				r_list_append (l, strdup ("")); // (ut64 *)-1);
				header.count--;
			}
			continue;
		}
		offset += sizeof (LE_entry_bundle_header);
		bool typeinfo = header.type & ENTRY_PARAMETER_TYPING_PRESENT;
		int i;
		for (i = 0; i < header.count; i++) {
			ut64 entry = -1;
			r_buf_read_at (bin->buf, offset, (ut8 *)&e, sizeof (e));
			switch (header.type & ~ENTRY_PARAMETER_TYPING_PRESENT) {
			case ENTRY16:
				if ((header.objnum - 1) < bin->header->objcnt) {
					entry = (ut64)e.entry_16.offset + bin->objtbl[header.objnum - 1].reloc_base_addr;
				}
				offset += sizeof (e.entry_16);
				if (typeinfo) {
					offset += (ut64)(e.entry_16.flags & ENTRY_PARAM_COUNT_MASK) * 2;
				}
				break;
			case CALLGATE:
				if ((header.objnum - 1) < bin->header->objcnt) {
					entry = (ut64)e.callgate.offset + bin->objtbl[header.objnum - 1].reloc_base_addr;
				}
				offset += sizeof (e.callgate);
				if (typeinfo) {
					offset += (ut64)(e.callgate.flags & ENTRY_PARAM_COUNT_MASK) * 2;
				}
				break;
			case ENTRY32:
				if ((header.objnum - 1) < bin->header->objcnt) {
					entry = (ut64)e.entry_32.offset + bin->objtbl[header.objnum - 1].reloc_base_addr;
				}
				offset += sizeof (e.entry_32);
				if (typeinfo) {
					offset += (ut64)(e.entry_32.flags & ENTRY_PARAM_COUNT_MASK) * 2;
				}
				break;
			case FORWARDER:
				offset += sizeof (e.forwarder);
				break;
			}
			if (entry != UT64_MAX) {
				r_list_append (l, r_str_newf ("0x%"PFMT64x, entry));
			}
		}
	}
	return l;
}