GF_Err trgt_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackGroupTypeBox *ptr = (GF_TrackGroupTypeBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->track_group_id = gf_bs_read_u32(bs);
	return GF_OK;
}