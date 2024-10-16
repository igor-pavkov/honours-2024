static void __create_iter_sections(RList *l, r_bin_le_obj_t *bin, RBinSection *sec, LE_object_page_entry *page, ut64 vaddr, int cur_page) {
	r_return_if_fail (l && bin && sec && page);
	LE_image_header *h = bin->header;
	ut32 offset = (h->itermap + (page->offset << (bin->is_le ? 0 : h->pageshift)));

	// Gets the first iter record
	ut16 iter_n = r_buf_read_ble16_at (bin->buf, offset, h->worder);
	offset += sizeof (ut16);
	ut16 data_size = r_buf_read_ble16_at (bin->buf, offset, h->worder);
	offset += sizeof (ut16);

	ut64 tot_size = 0;
	int iter_cnt = 0;
	ut64 bytes_left = page->size;
	while (iter_n && bytes_left > 0) {
		int i;
		for (i = 0; i < iter_n; i++) {
			RBinSection *s = R_NEW0 (RBinSection);
			if (!s) {
				break;
			}
			s->name = r_str_newf ("%s.page.%d.iter.%d", sec->name, cur_page, iter_cnt);
			s->bits = sec->bits;
			s->perm = sec->perm;
			s->size = data_size;
			s->vsize = data_size;
			s->paddr = offset;
			s->vaddr = vaddr;
			s->add = true;
			vaddr += data_size;
			tot_size += data_size;
			r_list_append (l, s);
			iter_cnt++;
		}
		bytes_left -= sizeof (ut16) * 2 + data_size;
		// Get the next iter record
		offset += data_size;
		iter_n = r_buf_read_ble16_at (bin->buf, offset, h->worder);
		offset += sizeof (ut16);
		data_size = r_buf_read_ble16_at (bin->buf, offset, h->worder);
		offset += sizeof (ut16);
	}
	if (tot_size < h->pagesize) {
		RBinSection *s = R_NEW0 (RBinSection);
		if (!s) {
			return;
		}
		s->name = r_str_newf ("%s.page.%d.iter.zerofill", sec->name, cur_page);
		s->bits = sec->bits;
		s->perm = sec->perm;
		s->vsize = h->pagesize - tot_size;
		s->vaddr = vaddr;
		s->add = true;
		r_list_append (l, s);
	}
}