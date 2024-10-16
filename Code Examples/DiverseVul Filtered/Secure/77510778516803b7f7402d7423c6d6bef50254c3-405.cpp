GF_Err trgt_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackGroupTypeBox *ptr = (GF_TrackGroupTypeBox *) s;
	if (!s) return GF_BAD_PARAM;
	s->type = ptr->group_type;
	e = gf_isom_full_box_write(s, bs);
	s->type = GF_ISOM_BOX_TYPE_TRGT;
	if (e) return e;
	gf_bs_write_u32(bs, ptr->track_group_id);
	return GF_OK;
}