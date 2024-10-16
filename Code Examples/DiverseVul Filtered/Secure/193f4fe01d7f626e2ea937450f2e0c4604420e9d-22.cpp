R_IPI RList *r_bin_file_get_strings(RBinFile *bf, int min, int dump, int raw) {
	r_return_val_if_fail (bf, NULL);
	RListIter *iter;
	RBinSection *section;
	RList *ret = dump? NULL: r_list_newf (r_bin_string_free);

	if (!raw && bf && bf->o && bf->o->sections && !r_list_empty (bf->o->sections)) {
		RBinObject *o = bf->o;
		r_list_foreach (o->sections, iter, section) {
			if (__isDataSection (bf, section)) {
				get_strings_range (bf, ret, min, raw, section->paddr,
						section->paddr + section->size, section);
			}
		}
		r_list_foreach (o->sections, iter, section) {
			/* load objc/swift strings */
			const int bits = (bf->o && bf->o->info) ? bf->o->info->bits : 32;
			const int cfstr_size = (bits == 64) ? 32 : 16;
			const int cfstr_offs = (bits == 64) ? 16 :  8;
			if (strstr (section->name, "__cfstring")) {
				int i;
				// XXX do not walk if bin.strings == 0
				ut8 *p;
				if (section->size > bf->size) {
					continue;
				}
				ut8 *sbuf = malloc (section->size);
				if (!sbuf) {
					continue;
				}
				r_buf_read_at (bf->buf, section->paddr + cfstr_offs, sbuf, section->size);
				for (i = 0; i < section->size; i += cfstr_size) {
					ut8 *buf = sbuf;
					p = buf + i;
					if ((i + ((bits == 64)? 8: 4)) >= section->size) {
						break;
					}
					ut64 cfstr_vaddr = section->vaddr + i;
					ut64 cstr_vaddr = (bits == 64) ? r_read_le64 (p) : r_read_le32 (p);
					RBinString *s = __stringAt (bf, ret, cstr_vaddr);
					if (s) {
						RBinString *bs = R_NEW0 (RBinString);
						if (bs) {
							bs->type = s->type;
							bs->length = s->length;
							bs->size = s->size;
							bs->ordinal = s->ordinal;
							bs->vaddr = cfstr_vaddr;
							bs->paddr = cfstr_vaddr; // XXX should be paddr instead
							bs->string = r_str_newf ("cstr.%s", s->string);
							r_list_append (ret, bs);
							ht_up_insert (o->strings_db, bs->vaddr, bs);
						}
					}
				}
				free (sbuf);
			}
		}
	} else {
		get_strings_range (bf, ret, min, raw, 0, bf->size, NULL);
	}
	return ret;
}