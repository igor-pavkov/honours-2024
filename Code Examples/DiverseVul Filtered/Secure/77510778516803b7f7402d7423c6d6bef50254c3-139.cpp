GF_Err mvcg_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_MultiviewGroupBox *ptr = (GF_MultiviewGroupBox *) s;
	ISOM_DECREASE_SIZE(s, 7)
	ptr->multiview_group_id = gf_bs_read_u32(bs);
	ptr->num_entries = gf_bs_read_u16(bs);
	gf_bs_read_u8(bs);
	ptr->entries = gf_malloc(ptr->num_entries * sizeof(MVCIEntry));
	memset(ptr->entries, 0, ptr->num_entries * sizeof(MVCIEntry));
	for (i=0; i<ptr->num_entries; i++) {
		ISOM_DECREASE_SIZE(s, 1)
		ptr->entries[i].entry_type = gf_bs_read_u8(bs);
		switch (ptr->entries[i].entry_type) {
		case 0:
			ISOM_DECREASE_SIZE(s, 4)
			ptr->entries[i].trackID = gf_bs_read_u32(bs);
			break;
		case 1:
			ISOM_DECREASE_SIZE(s, 6)
			ptr->entries[i].trackID = gf_bs_read_u32(bs);
			ptr->entries[i].tierID = gf_bs_read_u16(bs);
			break;
		case 2:
			ISOM_DECREASE_SIZE(s, 2)
			gf_bs_read_int(bs, 6);
			ptr->entries[i].output_view_id = gf_bs_read_int(bs, 10);
			break;
		case 3:
			ISOM_DECREASE_SIZE(s, 4)
			gf_bs_read_int(bs, 6)	;
			ptr->entries[i].start_view_id = gf_bs_read_int(bs, 10);
			ptr->entries[i].view_count = gf_bs_read_u16(bs);
			break;
		}
	}
	return gf_isom_box_array_read(s, bs);
}