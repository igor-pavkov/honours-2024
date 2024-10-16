GF_Err vwid_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_ViewIdentifierBox *ptr = (GF_ViewIdentifierBox *) s;
	ISOM_DECREASE_SIZE(s, 3)
	gf_bs_read_int(bs, 2);
	ptr->min_temporal_id = gf_bs_read_int(bs, 3);
	ptr->max_temporal_id = gf_bs_read_int(bs, 3);
	ptr->num_views = gf_bs_read_u16(bs);
	if (ptr->num_views > ptr->size / 6)
		return GF_ISOM_INVALID_FILE;

	ptr->views = gf_malloc(sizeof(ViewIDEntry)*ptr->num_views);
	memset(ptr->views, 0, sizeof(ViewIDEntry)*ptr->num_views);
	for (i=0; i<ptr->num_views; i++) {
		u32 j;
		ISOM_DECREASE_SIZE(s, 6)

		gf_bs_read_int(bs, 6);
		ptr->views[i].view_id = gf_bs_read_int(bs, 10);
		gf_bs_read_int(bs, 6);
		ptr->views[i].view_order_index = gf_bs_read_int(bs, 10);
		ptr->views[i].texture_in_stream = gf_bs_read_int(bs, 1);
		ptr->views[i].texture_in_track = gf_bs_read_int(bs, 1);
		ptr->views[i].depth_in_stream = gf_bs_read_int(bs, 1);
		ptr->views[i].depth_in_track = gf_bs_read_int(bs, 1);
		ptr->views[i].base_view_type = gf_bs_read_int(bs, 2);
		ptr->views[i].num_ref_views = gf_bs_read_int(bs, 10);

		if (ptr->views[i].num_ref_views > ptr->size / 2)
			return GF_ISOM_INVALID_FILE;

		ptr->views[i].view_refs = gf_malloc(sizeof(ViewIDRefViewEntry)*ptr->views[i].num_ref_views);
		for (j=0; j<ptr->views[i].num_ref_views; j++) {
			ISOM_DECREASE_SIZE(s, 2)
			gf_bs_read_int(bs, 4);
			ptr->views[i].view_refs[j].dep_comp_idc = gf_bs_read_int(bs, 2);
			ptr->views[i].view_refs[j].ref_view_id = gf_bs_read_int(bs, 10);
		}
	}
	return GF_OK;
}