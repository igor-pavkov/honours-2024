GF_Err mvcg_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_MultiviewGroupBox *ptr = (GF_MultiviewGroupBox *) s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;


	gf_bs_write_u32(bs, ptr->multiview_group_id);
	gf_bs_write_u16(bs, ptr->num_entries);
	gf_bs_write_u8(bs, 0);

	for (i=0; i<ptr->num_entries; i++) {
		gf_bs_write_u8(bs, ptr->entries[i].entry_type);
		switch (ptr->entries[i].entry_type) {
		case 0:
			gf_bs_write_u32(bs, ptr->entries[i].trackID);
			break;
		case 1:
			gf_bs_write_u32(bs, ptr->entries[i].trackID);
			gf_bs_write_u16(bs, ptr->entries[i].tierID);
			break;
		case 2:
			gf_bs_write_int(bs, 0, 6);
			gf_bs_write_int(bs, ptr->entries[i].output_view_id, 10);
			break;
		case 3:
			gf_bs_write_int(bs, 0, 6)	;
			gf_bs_write_int(bs, ptr->entries[i].start_view_id, 10);
			gf_bs_write_u16(bs, ptr->entries[i].view_count);
			break;
		}
	}
	return GF_OK;
}