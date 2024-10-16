static RList *get_strings(RBinFile *a, int min, int dump) {
	RListIter *iter;
	RBinSection *section;
	RBinObject *o = a? a->o: NULL;
	RList *ret;

	if (!o) {
		return NULL;
	}
	if (dump) {
		/* dump to stdout, not stored in list */
		ret = NULL;
	} else {
		ret = r_list_newf (r_bin_string_free);
		if (!ret) {
			return NULL;
		}
	}
	if (o->sections && !r_list_empty (o->sections) && !a->rawstr) {
		r_list_foreach (o->sections, iter, section) {
			if (is_data_section (a, section)) {
				get_strings_range (a, ret, min, section->paddr,
						section->paddr + section->size);
			}
		}
		r_list_foreach (o->sections, iter, section) {
			RBinString *s;
			RListIter *iter2;
			/* load objc/swift strings */
			const int bits = (a->o && a->o->info) ? a->o->info->bits : 32;
			const int cfstr_size = (bits == 64) ? 32 : 16;
			const int cfstr_offs = (bits == 64) ? 16 :  8;
			if (strstr (section->name, "__cfstring")) {
				int i;
// XXX do not walk if bin.strings == 0
				ut8 *p;
				for (i = 0; i < section->size; i += cfstr_size) {
					ut8 buf[32];
					if (!r_buf_read_at (
						    a->buf, section->paddr + i + cfstr_offs,
						    buf, sizeof (buf))) {
						break;
					}
					p = buf;
					ut64 cfstr_vaddr = section->vaddr + i;
					ut64 cstr_vaddr = (bits == 64)
								   ? r_read_le64 (p)
								   : r_read_le32 (p);
					r_list_foreach (ret, iter2, s) {
						if (s->vaddr == cstr_vaddr) {
							RBinString *new = R_NEW0 (RBinString);
							new->type = s->type;
							new->length = s->length;
							new->size = s->size;
							new->ordinal = s->ordinal;
							new->paddr = new->vaddr = cfstr_vaddr;
							new->string = r_str_newf ("cstr.%s", s->string);
							r_list_append (ret, new);
							break;
						}
					}
				}
			}
		}
	} else {
		get_strings_range (a, ret, min, 0, a->size);
	}
	return ret;
}