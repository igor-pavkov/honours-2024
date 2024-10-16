GF_Err vwid_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i, j;
	GF_ViewIdentifierBox *ptr = (GF_ViewIdentifierBox *) s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_int(bs, 0, 2);
	gf_bs_write_int(bs, ptr->min_temporal_id, 3);
	gf_bs_write_int(bs, ptr->max_temporal_id, 3);
	gf_bs_write_u16(bs, ptr->num_views);

	for (i=0; i<ptr->num_views; i++) {
		gf_bs_write_int(bs, 0, 6);
		gf_bs_write_int(bs, ptr->views[i].view_id, 10);
		gf_bs_write_int(bs, 0, 6);
		gf_bs_write_int(bs, ptr->views[i].view_order_index, 10);

		gf_bs_write_int(bs, ptr->views[i].texture_in_stream, 1);
		gf_bs_write_int(bs, ptr->views[i].texture_in_track, 1);
		gf_bs_write_int(bs, ptr->views[i].depth_in_stream, 1);
		gf_bs_write_int(bs, ptr->views[i].depth_in_track, 1);
		gf_bs_write_int(bs, ptr->views[i].base_view_type, 2);
		gf_bs_write_int(bs, ptr->views[i].num_ref_views, 10);

		for (j=0; j<ptr->views[i].num_ref_views; j++) {
			gf_bs_write_int(bs, 0, 4);
			gf_bs_write_int(bs, ptr->views[i].view_refs[j].dep_comp_idc, 2);
			gf_bs_write_int(bs, ptr->views[i].view_refs[j].ref_view_id, 10);
		}
	}
	return GF_OK;
}