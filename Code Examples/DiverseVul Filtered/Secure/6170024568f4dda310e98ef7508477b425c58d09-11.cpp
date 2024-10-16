GF_Err ilst_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 sub_type;
	GF_Box *a;
	GF_ItemListBox *ptr = (GF_ItemListBox *)s;
	while (ptr->size) {
		/*if no ilst type coded, break*/
		sub_type = gf_bs_peek_bits(bs, 32, 0);
		if (sub_type) {
			e = gf_isom_box_parse_ex(&a, bs, s->type, GF_FALSE);

			/* the macro will return in this case before we can free */
			if (!e && ptr->size < a->size) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isom] not enough bytes in box %s: %d left, reading %d (file %s, line %d)\n", gf_4cc_to_str(ptr->type), ptr->size, a->size, __FILE__, __LINE__ )); \
				e = GF_ISOM_INVALID_FILE;
			}
			if (e) {
				if (a) gf_isom_box_del(a);
				return e;
			}

			ISOM_DECREASE_SIZE(ptr, a->size);
			gf_list_add(ptr->child_boxes, a);
		} else {
			gf_bs_read_u32(bs);
			ISOM_DECREASE_SIZE(ptr, 4);
		}
	}
	return GF_OK;
}